
#include <set>
#include <string>
#include <utility>

#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/common/system/Clock.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/communication/messaging/AddressInUseException.hpp>
#include <tsd/communication/messaging/InvalidArgumentException.hpp>
#include <tsd/communication/messaging/Queue.hpp>

#include "QueueInternal.hpp"
#include "Router.hpp"
#include "Packet.hpp"
#include "utils.hpp"

using tsd::communication::event::TsdEvent;
using tsd::communication::event::IfcAddr_t;
using tsd::communication::event::LOOPBACK_ADDRESS;

namespace tsd { namespace communication { namespace messaging {

class IIfcNotifiy
{
public:
   virtual ~IIfcNotifiy();

   virtual void notifyDead(IfcAddr_t sender) = 0;
   virtual bool isSubscribed(uint32_t event) = 0;
};

class LocalIfc
   : public ILocalIfc
   , public IIfcNotifiy
{
   typedef struct {
      IfcAddr_t m_addr;
      TsdEvent *m_event;
      MonitorRef_t m_ref;
   } Monitor;
   typedef std::vector<Monitor> Monitors;

   Queue *m_queue;
   std::string m_interfaceName;
   const IMessageFactory &m_factory;
   Monitors m_monitors;
   tsd::common::system::Mutex m_lock;

public:
   LocalIfc(Queue *queue, const std::string &interfaceName, IfcAddr_t address,
            const IMessageFactory &factory);
   ~LocalIfc();

   void notifyDead(IfcAddr_t sender); // IIfcNotifiy
   bool isSubscribed(uint32_t event); // IIfcNotifiy

   void sendMessage(IfcAddr_t remoteIfc, std::auto_ptr<TsdEvent> msg);
   void broadcastMessage(std::auto_ptr<TsdEvent> msg);
   MonitorRef_t monitor(std::auto_ptr<TsdEvent> event, IfcAddr_t addr);
   void demonitor(MonitorRef_t monitor);

   Queue* getQueue() const { return m_queue; }
};

class RemoteIfc
   : public IRemoteIfc
   , public IIfcNotifiy
{
   typedef std::map<MonitorRef_t, TsdEvent*> Monitors;
   Queue *m_queue;
   std::string m_interfaceName;
   const IMessageFactory &m_factory;
   bool m_dead;
   Monitors m_monitors;
   tsd::common::system::Mutex m_lock;
   std::set<uint32_t> m_subscriptions;

public:
   RemoteIfc(Queue *queue, const std::string &interfaceName,
             IfcAddr_t localAddr, IfcAddr_t remoteAddr,
             const IMessageFactory &factory);
   ~RemoteIfc();

   void notifyDead(IfcAddr_t sender); // IIfcNotifiy
   bool isSubscribed(uint32_t event); // IIfcNotifiy

   void sendMessage(std::auto_ptr<TsdEvent> msg);
   MonitorRef_t monitor(std::auto_ptr<TsdEvent> event);
   void demonitor(MonitorRef_t monitor);
   void subscribe(uint32_t event);
   void subscribe(const std::vector<uint32_t>& events);
   void unsubscribe(uint32_t event);
   void unsubscribe(const std::vector<uint32_t>& events);

   Queue* getQueue() const { return m_queue; }
};


/*****************************************************************************/

/*
 * IIfcNotifiy
 */

IIfcNotifiy::~IIfcNotifiy()
{
}

/*
 * Local interface.
 */

LocalIfc::LocalIfc(Queue *queue, const std::string &interfaceName, IfcAddr_t address,
         const IMessageFactory &factory)
   : ILocalIfc(address)
   , m_queue(queue)
   , m_interfaceName(interfaceName)
   , m_factory(factory)
{
   m_queue->interfaceAdded(address, this, factory);
}

LocalIfc::~LocalIfc()
{
   if (!m_interfaceName.empty()) {
      m_queue->getRouter().unpublishInterface(m_interfaceName, getLocalIfcAddr());
   }
   m_queue->getRouter().freeIfcAddr(getLocalIfcAddr());
   m_queue->interfaceRemoved(getLocalIfcAddr());
   for (Monitors::iterator it(m_monitors.begin()); it != m_monitors.end(); ++it) {
      if (it->m_event != NULL) {
         delete it->m_event;
      } else {
         m_queue->purgeMessages(it->m_ref);
      }
   }
}

void LocalIfc::notifyDead(IfcAddr_t sender)
{
   tsd::common::system::MutexGuard g(m_lock);

   Monitors triggered;
   for (Monitors::iterator it(m_monitors.begin()); it != m_monitors.end(); ++it) {
      if (it->m_addr == sender && it->m_event != NULL) {
         triggered.push_back(*it);
         it->m_event = NULL;
      }
   }

   /*
    * Drop the lock before calling back into Queue. By design Queue.m_lock is
    * currently locked preventing races with demonitor() calls from other
    * threads.
    */
   g.unlock();
   for (Monitors::iterator it(triggered.begin()); it != triggered.end(); ++it) {
      m_queue->pushMessage(std::auto_ptr<TsdEvent>(it->m_event), it->m_ref);
   }
}

bool LocalIfc::isSubscribed(uint32_t /*event*/)
{
   return false;
}

void LocalIfc::sendMessage(IfcAddr_t remoteIfc, std::auto_ptr<TsdEvent> msg)
{
   m_queue->getRouter().sendUnicastMessage(getLocalIfcAddr(), remoteIfc, msg);
}

void LocalIfc::broadcastMessage(std::auto_ptr<TsdEvent> msg)
{
   m_queue->getRouter().sendBroadcastMessage(getLocalIfcAddr(), msg);
}

MonitorRef_t LocalIfc::monitor(std::auto_ptr<TsdEvent> event, IfcAddr_t addr)
{
   event->setSenderAddr(addr);
   event->setReceiverAddr(getLocalIfcAddr());

   tsd::common::system::MutexGuard g(m_lock);

   MonitorRef_t ref;
   bool found;
   do {
      g.unlock();
      ref = m_queue->makeRef();
      g.lock();
      found = false;
      for (Monitors::iterator it(m_monitors.begin()); it != m_monitors.end(); ++it) {
         if (it->m_ref == ref) {
            found = true;
            break;
         }
      }
   } while (found);
   g.unlock();

   // The user could register multiple messages for the same address.
   // joinGroup() can handle this.
   bool alive = m_queue->getRouter().joinGroup(addr, getLocalIfcAddr());
   Monitor m;
   m.m_addr = addr;
   m.m_ref = ref;
   if (alive) {
      m.m_event = event.release();
   } else {
      m.m_event = NULL;
      m_queue->pushMessage(event, ref);
   }

   g.lock();
   m_monitors.push_back(m);

   return ref;
}

void LocalIfc::demonitor(MonitorRef_t monitor)
{
   tsd::common::system::MutexGuard g(m_lock);

   Monitors::iterator it(m_monitors.begin());
   while (it != m_monitors.end()) {
      if (it->m_ref == monitor) {
         if (it->m_event != NULL) {
            delete it->m_event;
         }
         it = m_monitors.erase(it);
      } else {
         ++it;
      }
   }

   // purge queue after dropping the lock
   g.unlock();
   m_queue->purgeMessages(monitor);
}

/*
 * Remote interface.
 */

RemoteIfc::RemoteIfc(Queue *queue, const std::string &interfaceName,
          IfcAddr_t localAddr, IfcAddr_t remoteAddr,
          const IMessageFactory &factory)
   : IRemoteIfc(localAddr, remoteAddr)
   , m_queue(queue)
   , m_interfaceName(interfaceName)
   , m_factory(factory)
   , m_dead(false)
{
   m_queue->interfaceAdded(localAddr, this, factory);
}

RemoteIfc::~RemoteIfc()
{
   m_queue->getRouter().leaveGroup(getRemoteIfcAddr(), getLocalIfcAddr());
   m_queue->getRouter().freeIfcAddr(getLocalIfcAddr());
   m_queue->interfaceRemoved(getLocalIfcAddr());
   for (Monitors::iterator it(m_monitors.begin()); it != m_monitors.end(); ++it) {
      if (it->second != NULL) {
         delete it->second;
      } else {
         m_queue->purgeMessages(it->first);
      }
   }
}

void RemoteIfc::notifyDead(IfcAddr_t /*sender*/)
{
   Monitors triggered;

   tsd::common::system::MutexGuard g(m_lock);

   if (!m_dead) {
      for (Monitors::iterator it(m_monitors.begin()); it != m_monitors.end(); ++it) {
         if (it->second != NULL) {
            triggered.insert(*it);
            it->second = NULL;
         }
      }
      m_dead = true;
   }

   /*
    * Drop the lock before calling back into Queue. By design Queue.m_lock is
    * currently locked preventing races with demonitor() calls from other
    * threads.
    */
   g.unlock();
   for (Monitors::iterator it(triggered.begin()); it != triggered.end(); ++it) {
      m_queue->pushMessage(std::auto_ptr<TsdEvent>(it->second), it->first);
   }
}

bool RemoteIfc::isSubscribed(uint32_t event)
{
   tsd::common::system::MutexGuard g(m_lock);

   return m_subscriptions.find(event) != m_subscriptions.end();
}

void RemoteIfc::sendMessage(std::auto_ptr<TsdEvent> msg)
{
   m_queue->getRouter().sendUnicastMessage(getLocalIfcAddr(), getRemoteIfcAddr(), msg);
}

MonitorRef_t RemoteIfc::monitor(std::auto_ptr<TsdEvent> event)
{
   event->setSenderAddr(getRemoteIfcAddr());
   event->setReceiverAddr(getLocalIfcAddr());

   tsd::common::system::MutexGuard g(m_lock);

   MonitorRef_t ref;
   do {
      g.unlock();
      ref = m_queue->makeRef();
      g.lock();
   } while (m_monitors.find(ref) != m_monitors.end());

   if (m_dead) {
      m_monitors[ref] = NULL;
      g.unlock();
      m_queue->pushMessage(event, ref);
   } else {
      m_monitors[ref] = event.release();
   }

   return ref;
}

void RemoteIfc::demonitor(MonitorRef_t monitor)
{
   tsd::common::system::MutexGuard g(m_lock);

   Monitors::iterator it(m_monitors.find(monitor));
   if (it != m_monitors.end()) {
      if (it->second != NULL) {
         delete it->second;
      }
      m_monitors.erase(it);
   }

   // purge queue after dropping the lock
   g.unlock();
   m_queue->purgeMessages(monitor);
}

void RemoteIfc::subscribe(uint32_t event)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_subscriptions.insert(event);
}

void RemoteIfc::subscribe(const std::vector<uint32_t> &events)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_subscriptions.insert(events.begin(), events.end());
}

void RemoteIfc::unsubscribe(uint32_t event)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_subscriptions.erase(event);
}

void RemoteIfc::unsubscribe(const std::vector<uint32_t> &events)
{
   tsd::common::system::MutexGuard g(m_lock);
   for (std::vector<uint32_t>::const_iterator it(events.begin()); it != events.end(); ++it) {
      m_subscriptions.erase(*it);
   }
}

/*
 * Queue implementation.
 *
 * The queue is 
 */

Queue::Queue(const std::string &name, Router &router)
   : m_name(name)
   , m_log("tsd.communication.messaging.queue")
   , m_router(router)
   , m_numInterfaces(0)
   , m_refSeqNum(0)
{
}

Queue::~Queue()
{
   tsd::common::system::MutexGuard g(m_lock);
   if (m_numInterfaces > 0) {
      m_log << tsd::common::logging::LogLevel::Fatal
            << "Destroying queue " << m_name << " with active interfaces! Brace for impact!"
            << &std::endl;
   }
   for (EventQueue::iterator it(m_queue.begin()); it != m_queue.end(); ++it) {
      delete it->m_event;
   }
   for (Timers::iterator it(m_timers.begin()); it != m_timers.end(); ++it) {
      delete it->m_event;
   }
}

void Queue::interfaceAdded(IfcAddr_t ifc, IIfcNotifiy *notify, const IMessageFactory &factory)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_numInterfaces++;
   m_ifcFactories.insert(std::pair<tsd::communication::event::IfcAddr_t, const IMessageFactory*>(
      getInterface(ifc), &factory));
   m_ifcNotifications[getInterface(ifc)] = notify;
}

void Queue::interfaceRemoved(IfcAddr_t ifc)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_numInterfaces--;
   m_ifcFactories.erase(getInterface(ifc));
   m_ifcNotifications.erase(getInterface(ifc));

   // remove all messages that are for the removed interface
   EventQueue::iterator it(m_queue.begin());
   while (it != m_queue.end()) {
      if (it->m_event->getReceiverAddr() == ifc) {
         delete it->m_event;
         it = m_queue.erase(it);
      } else {
         ++it;
      }
   }
}

ILocalIfc *Queue::registerInterface(const IMessageFactory &factory,
                                    const std::string &interfaceName)
{
   IfcAddr_t addr = m_router.allocateIfcAddr(this);

   if (!interfaceName.empty()) {
      if (!m_router.publishInterface(interfaceName, addr)) {
         m_router.freeIfcAddr(addr);
         throw AddressInUseException(interfaceName);
      }
   }

   return new LocalIfc(this, interfaceName, addr, factory);
}

IRemoteIfc *Queue::connectInterface(tsd::communication::event::IfcAddr_t remoteAddr,
                                    const IMessageFactory &factory)
{
   IfcAddr_t localAddr = m_router.allocateIfcAddr(this);

   std::auto_ptr<RemoteIfc> ret(new RemoteIfc(this, "", localAddr, remoteAddr, factory));

   // join multicast group
   if (!m_router.joinGroup(remoteAddr, localAddr)) {
      ret->notifyDead(remoteAddr);
   }

   return ret.release();
}

IRemoteIfc *Queue::connectInterface(const std::string &interfaceName,
                                    const IMessageFactory &factory,
                                    uint32_t timeout)
{
   IfcAddr_t remoteAddr;

   if (m_router.lookupInterface(interfaceName, timeout, remoteAddr)) {
      IfcAddr_t localAddr = m_router.allocateIfcAddr(this);

      std::auto_ptr<RemoteIfc> ret(new RemoteIfc(this, interfaceName, localAddr, remoteAddr, factory));

      // join multicast group
      if (!m_router.joinGroup(remoteAddr, localAddr)) {
         ret->notifyDead(remoteAddr);
      }

      return ret.release();
   } else {
      return NULL;
   }
}

IRemoteIfc *Queue::connectInterface(const ILocalIfc *localIfc,
                                    const IMessageFactory &factory)
{
   const LocalIfc *local = dynamic_cast<const LocalIfc*>(localIfc);

   // must be from this implementation and from another queue
   if (local == NULL)
      throw InvalidArgumentException("incompatible implementation");
   if (local->getQueue() == this)
      throw InvalidArgumentException("same queue"); // FIXME: why?
   if (&local->getQueue()->m_router != &m_router)
      throw InvalidArgumentException("different router");

   IfcAddr_t localAddr = m_router.allocateIfcAddr(this);

   std::auto_ptr<RemoteIfc> ret(new RemoteIfc(this, "", localAddr, local->getLocalIfcAddr(), factory));

   // joint multicast group
   if (!m_router.joinGroup(local->getLocalIfcAddr(), localAddr)) {
      ret->notifyDead(local->getLocalIfcAddr());
   }

   return ret.release();
}

std::auto_ptr<TsdEvent> Queue::readMessage(uint32_t timeout, IMessageSelector *selector)
{
   tsd::common::system::MutexGuard g(m_lock);

   uint32_t startTime = 0;
   uint32_t nextTimer = 0;
   if (m_timers.empty()) {
      if (timeout != INFINITE_TIMEOUT) {
         startTime = tsd::common::system::Clock::getTickCounter();
      }
   } else {
      startTime = tsd::common::system::Clock::getTickCounter();
      nextTimer = checkTimerExpired(startTime);
   }

   std::auto_ptr<TsdEvent> ret;
   while ((ret = pullMessage(selector)).get() == NULL && timeout) {
      uint32_t loopTimeout = timeout;
      if (nextTimer && nextTimer < timeout) {
         loopTimeout = nextTimer;
      }

      if (loopTimeout != INFINITE_TIMEOUT) {
         m_queueCondition.wait(m_lock, loopTimeout);

         uint32_t endTime = tsd::common::system::Clock::getTickCounter();
         uint32_t elapsed = endTime - startTime;
         startTime = endTime;
         nextTimer = checkTimerExpired(endTime);

         if (timeout != INFINITE_TIMEOUT) {
            if (elapsed > timeout) {
               /*
                * Check again at least once instead of leaving the loop
                * immediately. There might be timer messages in the queue in
                * the meantime.
                */
               timeout = 0;
            } else {
               timeout -= elapsed;
            }
         }
      } else {
         m_queueCondition.wait(m_lock);

         /*
          * We have slept unconditionally. Either a message was put into the
          * queue or some other thread has added a timer to the queue. Check if
          * there is a timer and calculate when it is due.
          */
         if (!m_timers.empty()) {
            nextTimer = checkTimerExpired(tsd::common::system::Clock::getTickCounter());
         }
      }
   }

   return ret;
}

std::auto_ptr<TsdEvent> Queue::pullMessage(IMessageSelector *selector)
{
   std::auto_ptr<TsdEvent> ret;

   if (!m_queue.empty()) {
      if (selector == NULL) {
         ret.reset(m_queue.front().m_event);
         m_queue.pop_front();
      } else {
         for (EventQueue::iterator it(m_queue.begin()); it != m_queue.end(); ++it) {
            if (selector->filterEvent(it->m_event)) {
               ret.reset(it->m_event);
               m_queue.erase(it);
               break;
            }
         }
      }
   }

   return ret;
}

uint32_t Queue::checkTimerExpired(uint32_t now)
{
   uint32_t ret = 0;

   while (!m_timers.empty()) {
      Timers::iterator next(m_timers.begin());
      uint32_t nextTimer = next->m_expires;
      if (tsd::common::system::Clock::tickTimeAfterEq(now, nextTimer)) {
         bool rearm = next->m_interval != 0;

         EventSlot slot;
         slot.m_event = rearm ? next->m_event->clone() : next->m_event;
         slot.m_ref = next->m_ref;
         m_queue.push_back(slot);
         m_queueCondition.broadcast();

         if (rearm) {
            Timer t(*next);
            m_timers.erase(next);
            t.m_expires += t.m_interval;
            m_timers.insert(t);
         } else {
            m_timers.erase(next);
         }
      } else {
         ret = nextTimer - now;
         break;
      }
   }

   return ret;
}

void Queue::sendSelfMessage(std::auto_ptr<TsdEvent> message)
{
   message->setReceiverAddr(LOOPBACK_ADDRESS);
   pushMessage(message);
}

void Queue::pushMessage(std::auto_ptr<TsdEvent> message, uint32_t ref, bool multicast)
{
   tsd::common::system::MutexGuard g(m_lock);

   /*
    * Whenever a message comes in we first have to check for expired timers.
    * They have to be queued before this message.
    */
   if (!m_timers.empty()) {
      checkTimerExpired(tsd::common::system::Clock::getTickCounter());
   }

   /*
    * Check if multicast message should be delivered. If we get here we already
    * know that the receiving interface exists.
    */
   if (multicast) {
      if (!m_ifcNotifications[getInterface(message->getReceiverAddr())]
            ->isSubscribed(message->getEventId())) {
         m_log << tsd::common::logging::LogLevel::Trace
               << m_name << ": pushMessage(" << message->getEventId() << ") multicast dropped"
               << & std::endl;
         return;
      }
   }

   m_log << tsd::common::logging::LogLevel::Trace
         << m_name << ": pushMessage(" << message->getEventId() << ")" << & std::endl;

   EventSlot slot;
   slot.m_event = message.release();
   slot.m_ref = ref;
   m_queue.push_back(slot);
   m_queueCondition.broadcast();
   g.unlock();
}

bool Queue::pushPacket(std::auto_ptr<Packet> evt, bool multicast)
{
   tsd::common::system::MutexGuard g(m_lock);

   bool routed = false;
   InterfaceFactories::const_iterator it(m_ifcFactories.find(getInterface(evt->getReceiverAddr())));
   if (it != m_ifcFactories.end()) {
      std::auto_ptr<TsdEvent> msg(it->second->createEvent(evt->getEventId()));
      if (msg.get() != NULL) {
         m_log << tsd::common::logging::LogLevel::Trace
               << m_name << ": pushMessage(" << std::dec << evt->getEventId() << "): ok" << &std::endl;

         tsd::common::ipc::RpcBuffer buf;
         buf.init(evt->getBufferPtr(), evt->getBufferLength());
         msg->deserialize(buf);
         // TODO: check RpcBuffer underflow
         msg->setSenderAddr(evt->getSenderAddr());
         msg->setReceiverAddr(evt->getReceiverAddr());
         pushMessage(msg, 0, multicast);
         routed = true;
      } else {
         m_log << tsd::common::logging::LogLevel::Error
               << "pushMessage(" << m_name << ", " << std::dec << evt->getEventId() << "): NULL" << &std::endl;
      }
   } else {
      m_log << tsd::common::logging::LogLevel::Error
            << "pushMessage(" << m_name << ", " << std::dec << evt->getEventId() << "): LOST" << &std::endl;
   }

   return routed;
}

void Queue::purgeMessages(uint32_t ref)
{
   // ID 0 is always invalid as it is used for regular messages
   if (ref != 0u) {
      tsd::common::system::MutexGuard g(m_lock);

      EventQueue::iterator it(m_queue.begin());
      while (it != m_queue.end()) {
         if (it->m_ref == ref) {
            delete it->m_event;
            it = m_queue.erase(it);
         } else {
            ++it;
         }
      }
   }
}

TimerRef_t Queue::startTimer(std::auto_ptr<TsdEvent> event, uint32_t ms, bool cyclic)
{
   tsd::common::system::MutexGuard g(m_lock);

   // allocate unique reference
   TimerRef_t ref;
   bool found;
   do {
      found = false;
      ref = makeRef() | 1u;
      for (Timers::iterator it(m_timers.begin()); it != m_timers.end(); ++it) {
         if (it->m_ref == ref) {
            found = true;
            break;
         }
      }
   } while (found);

   // insert
   event->setReceiverAddr(LOOPBACK_ADDRESS);
   Timer t;
   t.m_event = event.release();
   t.m_expires = tsd::common::system::Clock::getTickCounter() + ms;
   t.m_interval = cyclic ? ms : 0;
   t.m_ref = ref;
   m_timers.insert(t);

   /*
    * Wake all readers. There is a timer and they have to adjust their timeout.
    */
   m_queueCondition.broadcast();

   return ref;
}

bool Queue::stopTimer(TimerRef_t timer)
{
   tsd::common::system::MutexGuard g(m_lock);

   // pending and expired?
   bool pending = false;
   Timers::iterator it(m_timers.begin());
   while (it != m_timers.end()) {
      if (it->m_ref == timer) {
         pending = (it->m_interval > 0) || tsd::common::system::Clock::tickTimeBefore(
            tsd::common::system::Clock::getTickCounter(), it->m_expires);
         delete it->m_event;
         m_timers.erase(it);
         break;
      } else {
         ++it;
      }
   }

   // timer message still in the queue?
   purgeMessages(timer);

   return pending;
}

/**
 * Create unique reference.
 *
 * The least significant bit is never set. It can be used to distinguish
 * different object types (monitors, timers).
 */
uint32_t Queue::makeRef()
{
   tsd::common::system::MutexGuard g(m_lock);

   uint32_t ret;
   do {
      ret = ++m_refSeqNum;
   } while (ret == 0);

   return ret << 1;
}

void Queue::dead(tsd::communication::event::IfcAddr_t sender,
                 tsd::communication::event::IfcAddr_t receiver)
{
   tsd::common::system::MutexGuard g(m_lock);

   InterfaceNotifications::const_iterator it(m_ifcNotifications.find(getInterface(receiver)));
   if (it != m_ifcNotifications.end()) {
      it->second->notifyDead(sender);
   }
}

// iternal stuff

/**
 * Router instance for local address space.
 */

IQueue *createQueue(const std::string &name)
{
   return new Queue(name, Router::getLocalRouter());
}

} } }
