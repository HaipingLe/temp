///////////////////////////////////////////////////////
//!\file CommunicationClient.cpp
//!\brief CommunicationClient core implementation
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <algorithm>

#include <tsd/common/ipc/commlib.h>
#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Clock.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/CommunicationClient.hpp>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/communication/TsdEventSerializer.hpp>
#include <tsd/communication/event/EventMasksAdm.hpp>
#include <tsd/communication/event/TsdTemplateEvent1.hpp>
#include <tsd/communication/event/TsdTemplateEvent2.hpp>

namespace {

   void split(std::vector<std::string> &tokens, const std::string &text, const char sep)
   {
      std::string::size_type start = 0, end = 0;

      while ((end = text.find(sep, start)) != std::string::npos) {
         tokens.push_back(text.substr(start, end - start));
         start = end + 1;
      }

      tokens.push_back(text.substr(start));
   }

}

namespace tsd { namespace communication {

typedef event::TsdTemplateEvent1<std::string> tHeloEvent;
typedef event::TsdTemplateEvent2<std::string, std::vector<uint32_t> > tVectorEvent;
typedef event::TsdTemplateEvent2<uint32_t, std::string> tDebugDisableRxTx;

CommunicationClient*       CommunicationClient::s_Instance(0);
tsd::common::system::Mutex CommunicationClient::s_InstanceMux;


class ComClientQueue : public IComClientQueue
                     , public tsd::common::system::Thread
{
public:
   ComClientQueue(const std::string &name, uint32_t timeout, CommunicationClient *client);
   ~ComClientQueue();

   // IComClientQueue
   bool addObserver(IComReceive* observer, const std::vector<uint32_t>& events);
   bool addObserver(IComReceive* observer, const uint32_t * pEvents, const uint32_t numEvents);
   bool deleteObserver(IComReceive* observer);

   // Thread
   void run();

   void queue(Buffer *buf);
   void checkWatchdog(uint32_t now);

   inline std::string getName() const
   {
      return m_name;
   }

   inline uint32_t getTimeout() const
   {
      return m_watchdogTimeout;
   }

   void makeActive()
   {
      m_Active = true;
   }

private:
   void doCheckWatchdog(uint32_t now);
   void notify(Buffer *buf);

   /* Cause the size of the vector don't reflect the actual
      interessted observers, it can also only contain NULL
      pointers, when its called in the notify callback.
      So we track the observers count seperately.
   */
   typedef std::pair<uint32_t , std::vector<IComReceive*> > tObserversElement;
   typedef std::map< uint32_t,  tObserversElement> tObservers;
   typedef std::map< IComReceive*, std::set<uint32_t> > tObserverEvents;

   std::string m_name;
   CommunicationClient *m_CC;
   tsd::communication::TsdEventSerializer* m_Serializer;

   tObservers m_Observers;
   tObserverEvents m_ObserverEvents;
   bool m_CallbackActive;
   bool m_CallbackObserversDeleted;

   tsd::common::system::Mutex m_ObserversLock;

   volatile bool m_running;
   std::queue< Buffer* > m_queue;
   tsd::common::system::Mutex m_queueLock;
   tsd::common::system::Semaphore m_queueDepth;

   bool m_watchdogFired;
   uint32_t m_watchdogTimeout;
   Buffer *m_currentEvent;
   uint32_t m_currentEventStart;

   /* Marks the Queue as active for event handling, affect cleaning up */
   bool m_Active;
};


ComClientQueue::ComClientQueue(const std::string &name, uint32_t timeout, CommunicationClient *cc)
   : tsd::common::system::Thread("tsd.communication.queue@" + name)
   , m_name(name)
   , m_CC(cc)
   , m_Serializer(tsd::communication::TsdEventSerializer::getInstance())
   , m_CallbackActive(false)
   , m_CallbackObserversDeleted(false)
   , m_running(true)
   , m_queueDepth(0)
   , m_watchdogFired(false)
   , m_watchdogTimeout(timeout)
   , m_currentEvent(NULL)
   , m_currentEventStart(0)
   , m_Active(false)
{
   start();
}

ComClientQueue::~ComClientQueue()
{
   // stop dispatcher thread
   m_queueLock.lock();
   m_running = false;
   m_queueDepth.up();
   m_queueLock.unlock();

   // wait for thread for finish
   join();
   m_CallbackActive = false;

   if(m_Active) {
      // deregister any remaining events
      std::vector<uint32_t> oldEvents;
      oldEvents.reserve(m_Observers.size());
      for (tObservers::iterator it(m_Observers.begin()); it != m_Observers.end(); ++it) {
         oldEvents.push_back(it->first);
      }
      m_CC->deregisterQueueEvents(this, oldEvents);
      m_CC->deregisterQueue(this);

      // free remaining events
      while (!m_queue.empty()) {
         m_queue.front()->deref();
         m_queue.pop();
      }
   }
}

bool ComClientQueue::addObserver(IComReceive* observer, const std::vector<uint32_t>& events)
{
   return (!events.empty()) ? addObserver(observer, &events[0], static_cast<uint32_t>(events.size())) : true;
}

/*
  AddObserver and deleteObserver can be called in the notify Callback.
  So we need to take care of modifyng observer lists, when we loop
  through it.
*/

bool ComClientQueue::addObserver(IComReceive* observer, const uint32_t * events, const uint32_t numEvents)
{
   std::vector<uint32_t> newEvents;

   tsd::common::system::MutexGuard guard(m_ObserversLock);

   std::set<uint32_t> &observerEvents = m_ObserverEvents[observer];
   for (int i = numEvents; i > 0; i--, events++) {
      uint32_t event = *events;
      if (observerEvents.insert(event).second) {
         tObservers::iterator it = m_Observers.find(event);
         if(it == m_Observers.end()) {
            std::pair<tObservers::iterator, bool> ret
               = m_Observers.insert( std::pair<uint32_t, tObserversElement>(event, tObserversElement(0u,  std::vector<IComReceive*>())) );
            it = ret.first;
         }
         std::vector<IComReceive*> &observers = it->second.second;
         uint32_t &count = it->second.first;

         if (count == 0) {
            // new event for this queue
            newEvents.push_back(event);
         }
         observers.push_back(observer);
         count++;
      }
   }

   bool ret = m_CC->registerQueueEvents(this, newEvents);

   if (!ret) {
      // TODO
   }

   return ret;
}

bool ComClientQueue::deleteObserver(IComReceive* observer)
{
   std::vector<uint32_t> oldEvents;

   tsd::common::system::MutexGuard guard(m_ObserversLock);

   // loop through all registered events of this observer
   std::set<uint32_t> &observerEvents = m_ObserverEvents[observer];
   for (std::set<uint32_t>::iterator it(observerEvents.begin()); it != observerEvents.end(); ++it) {
      uint32_t event = *it;

      // delete the observer for this event
      tObservers::iterator k = m_Observers.find(event);
      if( k == m_Observers.end() ) {
         continue;
      }

      std::vector<IComReceive*> &observers = k->second.second;
      uint32_t count;

      if( m_CallbackActive == true ) {
         count = 0;
         for(size_t i = 0; i < observers.size(); i++) {
            if(observers[i] == observer) {
               observers[i] = NULL;
               m_CallbackObserversDeleted = true;
            } else {
               count++;
            }
         }
         k->second.first = count; /* sync real observers count (!=0) */
      } else {
         observers.erase( std::remove( observers.begin(), observers.end(), observer ), observers.end() );
         count = static_cast<uint32_t>(observers.size());

         // is anybody still interested in this event?
         if (count == 0) {
            m_Observers.erase(k);
         } else {
            k->second.first = count; /* sync count */
         }
      }

      if(count == 0) {
         oldEvents.push_back(event);
      }
   }

   m_ObserverEvents.erase(observer);

   bool ret = m_CC->deregisterQueueEvents(this, oldEvents);

   if (!ret) {
      // TODO
   }

   return ret;
}

void ComClientQueue::queue(Buffer *buf)
{
   buf->ref();

   tsd::common::system::MutexGuard queueGuard(m_queueLock);
   m_queue.push(buf);
   if (m_currentEvent == NULL) {
      /*
       * This will be our next event. Put it already into m_currentEvent to
       * activate the watchdog timeout for it. Otherwise we would no get a
       * watchdog reset if the queue thread is starved for long times.
       */
      m_currentEvent = buf;
      m_currentEventStart = 0;
   }
   queueGuard.unlock();

   m_queueDepth.up();
}

void ComClientQueue::checkWatchdog(uint32_t now)
{
   tsd::common::system::MutexGuard queueGuard(m_queueLock);
   doCheckWatchdog(now);
}

void ComClientQueue::run()
{
   for (;;) {
      // wait for next event
      m_queueDepth.down();

      tsd::common::system::MutexGuard queueGuard(m_queueLock);
      if (!m_running) {
         break;
      }

      // get next event
      if (!m_queue.empty()) {
         Buffer *buf = m_queue.front();
         m_queue.pop();
         m_currentEvent = buf;
         m_currentEventStart = tsd::common::system::Clock::getTickCounter();
         doCheckWatchdog(m_currentEventStart);
         queueGuard.unlock();

         notify(buf);

         // check watchdog again
         queueGuard.lock();
         doCheckWatchdog(tsd::common::system::Clock::getTickCounter());
         m_currentEvent = NULL;

         // free event
         buf->deref();
      }
   }
}

void ComClientQueue::notify(Buffer *buf)
{
   tsd::common::ipc::RpcBuffer rpcBuf;
   rpcBuf.init((char*)buf->payload(), buf->length());

   std::auto_ptr<event::TsdEvent> event = m_Serializer->deserialize(rpcBuf);
   if (event.get() != NULL) {
      /*
       * Go through the list of observers. If we find one who wants this event,
       * note it. If we find another one, dispatch a copy of the event to the
       * previous one and keep the new one. In contrast to the simpler solution
       * of dispatching a copy to each observer as we encounter them (and
       * finally discarding the original), this saves one copy per dispatch.
       */
      IComReceive* found = NULL;
      /* cause of lazy deleting, in the end found can be zero, although we had a observer, so for the warning below use this bool */
      bool observerFound = false;
      uint32_t id = event->getEventId();
      {
         tsd::common::system::MutexGuard guard(m_ObserversLock);

         m_CallbackActive = true;
         m_CallbackObserversDeleted = false;
         /*
          * We might accidentially grow m_Observers but it's just not worth
          * checking this before because it should not happen at all.
          *
          * Update: This assumption is just plain wrong, because it will invalidate
          * all iterators. So we are using indicies. And consider removal of
          * observers too.
          */

         tObservers::iterator it = m_Observers.find(id);
         if(it != m_Observers.end()) {
            std::vector<IComReceive*> &observers = it->second.second;
            uint32_t &count = it->second.first;

            /* use indicies and not iterators, cause the iterators can be invalidated
               in the callback. Also the vector can increase, but never decrease in
               size if the callback is active.
            */
            size_t vector_size = observers.size();
            for (size_t i = 0; i < vector_size; i++) {
               if (found != NULL) {
                  found->receiveEvent(std::auto_ptr<event::TsdEvent>(event->clone()));
                  observerFound = true;
               }
               found = observers[i];
            }

            if (found != NULL) {
               found->receiveEvent(event);
               observerFound = true;
            }

            if (m_CallbackObserversDeleted ) {
               /* clean up deleted observers in the callback */
               observers.erase( std::remove( observers.begin(), observers.end(), static_cast<IComReceive*> (NULL) ), observers.end() );
               count = static_cast<uint32_t>(observers.size());
               if (count == 0) {
                  m_Observers.erase(it);
               }
            }
         }

         m_CallbackActive = false;
      }

      if (observerFound == false) {
         *m_CC->getLogger() << tsd::common::logging::LogLevel::Error
            << "ComClientQueue::notify: no observer found for EventID "
            << std::hex << id << "(" << buf->eventId()
            << ") on queue '" << m_name << "'" << &std::endl;
      }
   } else {
      *m_CC->getLogger() << tsd::common::logging::LogLevel::Error
         << "ComClientQueue::notify: no deserializer found for EventID "
         << std::hex << buf->eventId() << &std::endl;
   }
}

void ComClientQueue::doCheckWatchdog(uint32_t now)
{
   // report only for the first event
   if (!m_watchdogFired && m_currentEvent != NULL) {
      uint32_t timeout = m_currentEvent->m_timestamp + m_watchdogTimeout;
      if (tsd::common::system::Clock::tickTimeBefore(timeout, now)) {
         m_CC->watchdogExpired(m_name, m_currentEvent->eventId(),
            m_currentEvent->m_timestamp, m_currentEventStart, timeout, now);
         m_watchdogFired = true;
      }
   }
}

/*****************************************************************************/

CommunicationClient::CommunicationClient(void)
   : m_Connection(NULL)
   , m_Serializer(tsd::communication::TsdEventSerializer::getInstance())
{
   m_Log = new tsd::common::logging::Logger("tsd.communication.comclient");

   m_SndBufferSize = 32768 / 4;
   m_SndBuffer = new uint32_t[m_SndBufferSize];

   // get environment overrides
   const char *wdEnv = std::getenv("TSD_COMCLIENT_WATCHDOG");
   if (wdEnv != NULL) {
      std::vector<std::string> args;
      split(args, wdEnv, ',');
      for (std::vector<std::string>::iterator it(args.begin()); it != args.end(); ++it) {
         std::vector<std::string> setting;
         split(setting, *it, '=');
         if (setting.size() == 2) {
            m_queueTimeouts[setting[0]] = std::atoi(setting[1].c_str());
         }
      }
   }

   // create default queue
   m_defaultQueue = new ComClientQueue("default", getQueueTimeout("default", BulkQueue), this);
   m_defaultQueue->makeActive();
   m_allQueues["default"] = m_defaultQueue;
}

CommunicationClient::~CommunicationClient(void)
{
   if (m_Connection) {
      delete m_Connection;
   }
   delete m_defaultQueue;
   // its not necessary to remove the queue from the map
   delete [] m_SndBuffer;
   delete m_Log;
}

CommunicationClient* CommunicationClient::getInstance(void)
{
   if (s_Instance == NULL) {
      tsd::common::system::MutexGuard guard(s_InstanceMux);
      if (s_Instance == NULL) {
         s_Instance = new CommunicationClient();
      }
   }

   return s_Instance;
}

void CommunicationClient::destroyInstance(void)
{
   tsd::common::system::MutexGuard guard(s_InstanceMux);

   if (s_Instance) {
      delete s_Instance;
      s_Instance = NULL;
   }
}

void CommunicationClient::init(const std::string& receiverName, const char_t * server, bool implicitConnect)
{
   tsd::common::system::MutexGuard guard(m_InitMux);

   if (!m_Connection) {
      m_Connection = client::Connection::openConnection(this, *m_Log, server);

      tHeloEvent* heloEvent( new tHeloEvent(event::TSDEVENTID_ADM_HELO) );
      heloEvent->setData1(receiverName);
      send(std::auto_ptr<event::TsdEvent>(heloEvent));

      /*
       * Backwards compatibility hack: connect IPC library (almost) the same
       * way as the old ComMgr did. If..
       *
       *   - server == NULL: connect via shm
       *   - server == "LOCAL": don't connect ipc, server is in same address space
       *   - server without schema (xxx://...): interpret as IP-Address and connect ipc via tcp
       */
      if (implicitConnect) {
         if (server == NULL) {
/* on QNX use the new ipc backend */
#if defined(TARGET_OS_POSIX_QNX) and (TSD_COMMON_API_VERSION >= 210)
            tsd::common::ipc::createConnectServer("qnx:root", 0);
#else
            tsd::common::ipc::createConnectServer("127.0.0.1", 5555);
#endif
         } else if (std::strcmp(server, "LOCAL") != 0 &&
                    std::strstr(server, "://") == NULL) {
            tsd::common::ipc::createConnectServer(server, 5555);
         }
      }
   }
}

IComClientQueue* CommunicationClient::getQueue(const std::string &name, QueueType type)
{
   ComClientQueue *queue = NULL;
   ComClientQueue *newqueue = NULL;
   uint32_t timeout;
   {
      tsd::common::system::MutexGuard guard(m_QueuesLock);
      timeout = getQueueTimeout(name, type);
      tAllQueues::iterator it = m_allQueues.find(name);
      if (it != m_allQueues.end()) {
         queue = it->second;
      }
   }

   if (queue == NULL) {
      /* create the queue without holding a critical lock, to avoid the famous WAITTHREAD-starvations */
      newqueue = new ComClientQueue(name, timeout, this);
      {
         tsd::common::system::MutexGuard guard(m_QueuesLock);

         tAllQueues::iterator it = m_allQueues.find(name);
         if (it == m_allQueues.end()) {
            newqueue->makeActive();
            m_allQueues[name] = newqueue;
         } else {  /* someone was faster than we */
            queue = it->second;
         }
      }

      if(queue) { /* if someone was faster, delete the tmp ComQueue */
         delete newqueue;
         newqueue = NULL;
      } else {
         queue = newqueue;
      }
   }
   if (newqueue == NULL) {
      if (queue->getTimeout() != timeout) {
         *m_Log << tsd::common::logging::LogLevel::Warn
                << "Different timeouts specified for queue '" << name << "': "
                << queue->getTimeout() << "ms vs. " << timeout << "ms"
                << std::endl;
      }
   }

   return queue;
}

bool CommunicationClient::send(std::auto_ptr<tsd::communication::event::TsdEvent> tsdevent)
{
   bool ret = false;

   if (m_Connection) {
      tsd::common::ipc::RpcBuffer rpcBuf;
      tsd::common::system::MutexGuard guard(m_SndBufferMux);

      do {
         rpcBuf.init((char*) m_SndBuffer, m_SndBufferSize);
         m_Serializer->serialize(rpcBuf, *tsdevent.get());
         if (rpcBuf.didOverflow()) {
            delete [] m_SndBuffer;
            m_SndBufferSize *= 2;
            m_SndBuffer = new uint32_t[m_SndBufferSize];
         }
      } while (rpcBuf.didOverflow());

      ret = m_Connection->send(m_SndBuffer, static_cast<uint32_t>(rpcBuf.getSize()));
   }

   return ret;
}

void CommunicationClient::messageReceived(const void *buf, uint32_t len)
{
   Buffer *msg = allocBuffer(len);
   msg->fill(buf, len);
   msg->m_timestamp = tsd::common::system::Clock::getTickCounter();

   uint32_t id = msg->eventId();
   if ((id & event::TSDEVENT_MASK) != event::OFFSET_TSDEVENT_ADM) {
      // dispatch to all queues
      tsd::common::system::MutexGuard guard(m_QueuesLock);

      tQueues::mapped_type &queues = m_Queues[id];
      for (tQueues::mapped_type::iterator it(queues.begin()); it != queues.end(); ++it) {
         (*it)->queue(msg);
      }
   } else {
      tsd::common::ipc::RpcBuffer rpcBuf;
      rpcBuf.init((char*)buf, len);

      uint32_t buf[2];
      tsd::common::ipc::RpcBuffer replyRpcBuf;
      replyRpcBuf.init((char*)buf, sizeof(buf));

      rpcBuf >> id; // discard event id
      switch (id) {
         case event::TSDEVENTID_ADM_PING:
         {
            // check all queues
            {
               uint32_t now = tsd::common::system::Clock::getTickCounter();
               tsd::common::system::MutexGuard guard(m_QueuesLock);
               for (tAllQueues::iterator it(m_allQueues.begin()); it != m_allQueues.end(); ++it) {
                  it->second->checkWatchdog(now);
               }
            }

            // send ping back
            uint32_t challenge;
            rpcBuf >> challenge;

            replyRpcBuf << event::TSDEVENTID_ADM_PONG;
            replyRpcBuf << ++challenge;
            m_Connection->send(buf, static_cast<uint32_t>(replyRpcBuf.getSize()));
            break;
         }
         default:
            *m_Log << tsd::common::logging::LogLevel::Warn
                   << "Unhandled admin event: 0x" << std::hex << id
                   << std::endl;
      }
   }

   msg->deref();
}

void CommunicationClient::disconnected()
{
   *m_Log << tsd::common::logging::LogLevel::Error
          << "CommunicationClient disconnected! Communication dead..."
          << std::endl;
}

void CommunicationClient::watchdogExpired(const std::string &name, uint32_t id,
   uint32_t timeEntered, uint32_t timeStarted, uint32_t timeWd, uint32_t now)
{
   uint32_t buf[64];
   tsd::common::ipc::RpcBuffer rpcBuf;
   rpcBuf.init((char*)buf, sizeof(buf));

   rpcBuf << event::TSDEVENTID_ADM_WATCHDOG_EXPIRED;
   rpcBuf << name;
   rpcBuf << id;
   rpcBuf << timeEntered;
   rpcBuf << timeStarted;
   rpcBuf << timeWd;
   rpcBuf << now;
   m_Connection->send(buf, static_cast<uint32_t>(rpcBuf.getSize()));

   *m_Log << tsd::common::logging::LogLevel::Fatal
          << "\033[41mCommunicationClient watchdog: queue: '" << name
          << "' event: 0x" << std::hex << id
          << " entered: " << std::dec << timeEntered
          << " started: " << timeStarted << " deadline: " << timeWd
          << " now: " << now << "\033[0m" << std::endl;
}

bool CommunicationClient::addObserver(IComReceive* observer, const std::vector<uint32_t>& events)
{
   return m_defaultQueue->addObserver(observer, events);
}

bool CommunicationClient::addObserver(IComReceive* observer, const uint32_t * events, const uint32_t numEvents)
{
   return m_defaultQueue->addObserver(observer, events, numEvents);
}

bool CommunicationClient::deleteObserver(IComReceive* observer)
{
   return m_defaultQueue->deleteObserver(observer);
}

bool CommunicationClient::registerQueueEvents(ComClientQueue *queue, const std::vector<uint32_t> &events)
{
   bool ret = !!m_Connection;

   std::vector<uint32_t> newEvents;
   newEvents.reserve(events.size());

   if (ret) {
      tsd::common::system::MutexGuard guard(m_QueuesLock);

      for (std::vector<uint32_t>::const_iterator evIt(events.begin()); evIt != events.end(); ++evIt) {
         uint32_t event = *evIt;
         tQueues::mapped_type &queues = m_Queues[event];
         if (queues.empty()) {
            newEvents.push_back(event);
         }
         queues.push_back(queue);
      }
   }

   if (!newEvents.empty()) {
      tVectorEvent* sndev( new tVectorEvent(event::TSDEVENTID_ADM_REGISTER_CC_EVENTS) );
      sndev->setData1("foo");
      sndev->setData2(newEvents);
      ret = send(std::auto_ptr<event::TsdEvent>(sndev));
   }

   // FIXME: revert if ret == false

   return ret;
}

bool CommunicationClient::deregisterQueueEvents(ComClientQueue *queue, const std::vector<uint32_t> &events)
{
   bool ret = !!m_Connection;

   std::vector<uint32_t> oldEvents;
   oldEvents.reserve(events.size());

   if (ret) {
      tsd::common::system::MutexGuard guardOb(m_QueuesLock);

      for (std::vector<uint32_t>::const_iterator evIt(events.begin()); evIt != events.end(); ++evIt) {
         uint32_t event = *evIt;

         // delete the queue for this event
         tQueues::mapped_type &queues = m_Queues[event];
         for(tQueues::mapped_type::iterator qIt = queues.begin(); qIt != queues.end(); ++qIt)
         {
            if(*qIt == queue)
            {
               queues.erase(qIt);
               break;
            }
         }

         if (queues.empty()) {
            oldEvents.push_back(event);
         }
      }
   }

   if (!oldEvents.empty()) {
      tVectorEvent* sndev( new tVectorEvent(event::TSDEVENTID_ADM_DEREGISTER_CC_EVENTS) );
      sndev->setData1("foo");
      sndev->setData2(oldEvents);
      ret = send(std::auto_ptr<event::TsdEvent>(sndev));
   }

   // FIXME: revert if ret == false

   return ret;
}

void CommunicationClient::deregisterQueue(ComClientQueue *queue)
{
   tsd::common::system::MutexGuard guard(m_QueuesLock);
   m_allQueues.erase(queue->getName());
}

bool
CommunicationClient::setDebugMode(DebugFunc type, uint32_t param1, void * param2)
{
   bool ret = false;

   switch (type) {
      case SET_DOGGY:
         ret = true;
         break;

      case DISABLE_RECEIVE:
      {
         tDebugDisableRxTx* sndev( new tDebugDisableRxTx(event::TSDEVENTID_ADM_DEBUG_DISABLE_RX) );
         sndev->setData1(param1);
         sndev->setData2(std::string(static_cast<char*>(param2)));
         ret = send(std::auto_ptr<event::TsdEvent>(sndev));
         break;
      }

      case DISABLE_TRANSMIT:
      {
         tDebugDisableRxTx* sndev( new tDebugDisableRxTx(event::TSDEVENTID_ADM_DEBUG_DISABLE_TX) );
         sndev->setData1(param1);
         sndev->setData2(std::string(static_cast<char*>(param2)));
         ret = send(std::auto_ptr<event::TsdEvent>(sndev));
         break;
      }
   }
   return ret;
}

uint32_t
CommunicationClient::getQueueTimeout(const std::string &name, QueueType type) const
{
   uint32_t result;

   if (m_queueTimeouts.find(name) != m_queueTimeouts.end()) {
      result = m_queueTimeouts.find(name)->second;
   } else {
      switch (type) {
      case RealtimeQueue:
         result = 2;
         break;
      case NormalQueue:
         result = 10;
         break;
      case LongRunQueue:
         result = 120;
         break;
      case BulkQueue:
      default:
         result = 30;
         break;
      }
   }

   return result * 1000;
}

} /* namespace communication */ } /* namespace tsd */
