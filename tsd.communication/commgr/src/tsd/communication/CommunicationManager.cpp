/**
 * \file CommunicationManager.cpp
 * \brief Communication Manager implementation
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */

#include <algorithm>

#include <tsd/common/errors/ConnectException.hpp>
#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/Client.hpp>
#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/communication/DownstreamManager.hpp>
#include <tsd/communication/event/EventMasksAdm.hpp>

#ifdef TARGET_OS_POSIX_QNX
  #include <tsd/communication/SendReceiveBackend.hpp>
#endif
#if defined(TARGET_OS_POSIX)
  #include <tsd/communication/TcpBackend.hpp>
#elif defined(TARGET_OS_WIN32)
  #include <tsd/communication/TcpBackendWindows.hpp>
#endif

using tsd::communication::CommunicationManager;
using namespace tsd::communication;
using namespace tsd::communication::client;

namespace {

   void extractEvents(Buffer *msg, std::vector<uint32_t> &vec)
   {
      vec.clear();

      tsd::common::ipc::RpcBuffer buf;
      buf.init((char*)msg->payload(), msg->length());
      buf.getInt(); // discard event id
      buf.getString(); // discard dummy receiver name
      buf >> vec;
   }

}

tsd::communication::Backend::~Backend()
{
}


CommunicationManager::CommunicationManager()
   : tsd::common::system::Thread("tsd.communication.commgr.watchdog")
   , m_log("tsd.communication.commgr")
   , m_upstreamClient(NULL)
   , m_downstreamManager(NULL)
   , m_watchdogCallback(NULL)
   , m_watchdogSemaphore(0)
{
   m_watchdogBlackList.push_back("HMI");
}

CommunicationManager::~CommunicationManager()
{
   if (m_watchdogCallback) {
      lock();
      m_watchdogCallback = NULL;
      m_watchdogSemaphore.up();
      unlock();
      join();
   }

   if (m_downstreamManager) {
      delete m_downstreamManager;
   }

   for (std::list<Backend*>::iterator i = m_backends.begin(); i != m_backends.end(); ++i) {
      delete *i;
   }
}

void CommunicationManager::addBackend(const std::string &address)
{
   Backend *backend = NULL;
   std::string url(address);

   static const char prefixTcp[] = "tcp://";

#ifdef TARGET_OS_POSIX_QNX
   static const char prefixQnx[]    = "qnx://";
   static const char prefixLocal[]  = "local/";
   static const char prefixGlobal[] = "global/";

   if (url.compare(0, std::strlen(prefixQnx), prefixQnx, std::strlen(prefixQnx)) == 0) {
      url = url.substr(6);
      bool global = false;

      if (url.compare(0, std::strlen(prefixLocal), prefixLocal, std::strlen(prefixLocal)) == 0) {
         url = url.substr(6);
      } else if (url.compare(0, std::strlen(prefixGlobal), prefixGlobal, std::strlen(prefixGlobal)) == 0) {
         global = true;
         url = url.substr(7);
      } else {
         throw tsd::common::errors::ConnectException("Neither local nor global QNX protocol");
      }

      backend = new SendReceiveBackend(*this, m_log, url, global);
   } else
#endif
#if defined(TARGET_OS_POSIX) || defined(TARGET_OS_WIN32)
   if (url.compare(0, std::strlen(prefixTcp), prefixTcp, std::strlen(prefixTcp)) == 0) {
      backend = new TcpBackend(*this, m_log, url.substr(6));
   } else if (url.find("://") == std::string::npos) {
      // Assume tcp if no schema is specified
      backend = new TcpBackend(*this, m_log, url);
   } else
#else
   (void)prefixTcp;
#endif
   {
      std::string msg("Invalid protocol: ");
      msg += url;
      throw tsd::common::errors::ConnectException(msg);
   }

   m_backends.push_back(backend);
}

void CommunicationManager::connectDownstreamCM(const std::string &address)
{
   if (m_downstreamManager) {
      throw tsd::common::errors::ConnectException("Already connected");
   }

   m_downstreamManager = new DownstreamManager(*this, m_log, address);

   // announce us as an upstream Manager
   Buffer *buf = allocBuffer(16);
   tsd::common::ipc::RpcBuffer rpc;
   rpc.init((char*)buf->payload(), 16);
   rpc.storeInt(tsd::communication::event::TSDEVENTID_ADM_INIT_CM_EVENTS);
   assert(!rpc.didOverflow());
   m_downstreamManager->pushMessage(buf);
   buf->deref();

   // tell him all our registered events
   std::set<uint32_t> allEvents;
   for (std::map<uint32_t, std::set<Client*> >::iterator i = m_events.begin();
        i != m_events.end(); ++i) {
      allEvents.insert(i->first);
   }
   informEvents(m_downstreamManager, allEvents,
      tsd::communication::event::TSDEVENTID_ADM_REGISTER_CC_EVENTS);
}

void CommunicationManager::registerTimeOutHandler(IComWatchdog *callback)
{
   if (!m_watchdogCallback) {
      m_watchdogCallback = callback;
      start(); // start watchdog thread
   }
}

void CommunicationManager::run()
{
   while (m_watchdogCallback) {
      m_watchdogSemaphore.down(1000);
      lock();
      if (m_watchdogCallback) {
         checkWatchdog();
      }
      unlock();
   }
}

void CommunicationManager::setDebug(bool /*enable*/)
{

}

void CommunicationManager::setWatchdogBlacklist(const std::vector<std::string> &blackList)
{
   lock();
   m_watchdogBlackList = blackList;
   unlock();
}

void CommunicationManager::registerClient(Client *client)
{
   m_clients.push_back(client);
}

void CommunicationManager::deregisterClient(Client *client)
{
   for (std::map<uint32_t, uint32_t>::iterator it = client->events.begin();
        it != client->events.end(); ++it) {
      std::set<Client*> &clients = m_events[it->first];
      clients.erase(client);
      if (clients.empty())
         m_events.erase(it->first);
   }

   if (m_upstreamClient == client) {
      m_upstreamClient = NULL;
      m_upstreamEvents.clear();
   }

   m_clients.erase(std::find(m_clients.begin(), m_clients.end(), client));
}

void CommunicationManager::dispatchMessage(Client *client, Buffer *msg)
{
   uint32_t event = msg->eventId();

   if ((event & tsd::communication::event::TSDEVENT_MASK) !=
         tsd::communication::event::OFFSET_TSDEVENT_ADM) {
      if (client->transmitEnabled) {
         forwardMessage(client, event, msg);
      }
   } else {
      switch (event) {
         case tsd::communication::event::TSDEVENTID_ADM_REGISTER_CC_EVENTS:
            handleRegisterClient(client, msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CC_EVENTS:
            handleDeregisterClient(client, msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_REGISTER_CM_EVENTS:
            handleRegisterDownstream(msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CM_EVENTS:
            handleDeregisterDownstream(msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_HELO:
            handleHelo(client, msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_INIT_CM_EVENTS:
            handleUpstreamClient(client);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_RX:
            handleDisableRx(msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_TX:
            handleDisableTx(msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_PING:
            handlePing(client, msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_PONG:
            handlePong(client, msg);
            break;
         case tsd::communication::event::TSDEVENTID_ADM_WATCHDOG_EXPIRED:
            handleWatchdog(client, msg);
            break;
         default: break;
      }
   }

   msg->deref();
}

void CommunicationManager::handleRegisterClient(Client *client, Buffer *msg)
{
   std::set<uint32_t> newEvents;

   std::vector<uint32_t> events;
   extractEvents(msg, events);

   if (!client->isManager) {
      for (std::vector<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
         uint32_t newEvent = *it;

         m_log << tsd::common::logging::LogLevel::Trace
               << "registerClient " << client->name << " " << newEvent << std::endl;

         uint32_t numReg = client->events[newEvent] + 1;
         client->events[newEvent] = numReg;
         if (numReg == 1) {
            std::set<Client*> &clients = m_events[newEvent];
            if (clients.empty() && (m_upstreamClient || m_downstreamManager)) {
               newEvents.insert(newEvent);
            }
            m_events[newEvent].insert(client);
         }
      }

      if (!newEvents.empty()) {
         if (m_upstreamClient) {
            informEvents(m_upstreamClient, newEvents,
               tsd::communication::event::TSDEVENTID_ADM_REGISTER_CM_EVENTS);
         }
         if (m_downstreamManager) {
            informEvents(m_downstreamManager, newEvents,
               tsd::communication::event::TSDEVENTID_ADM_REGISTER_CC_EVENTS);
         }
      }
   } else {
      for (std::vector<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
         uint32_t newEvent = *it;

         m_log << tsd::common::logging::LogLevel::Trace
               << "registerClient <upstreamCM> " << newEvent << std::endl;

         m_upstreamEvents.insert(newEvent);
      }
   }
}

void CommunicationManager::handleDeregisterClient(Client *client, Buffer *msg)
{
   std::set<uint32_t> oldEvents;

   std::vector<uint32_t> events;
   extractEvents(msg, events);

   if (!client->isManager) {
      for (std::vector<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
         uint32_t oldEvent = *it;

         m_log << tsd::common::logging::LogLevel::Trace
               << "deregisterClient " << client->name << " " << oldEvent << std::endl;

         int32_t numReg = static_cast<int32_t>(client->events[oldEvent]) - 1;
         if (numReg <= 0) {
            client->events.erase(oldEvent);

            std::set<Client*> &clients = m_events[oldEvent];
            clients.erase(client);
            if (clients.empty()) {
               m_events.erase(oldEvent);
               if (m_upstreamClient || m_downstreamManager) {
                  oldEvents.insert(oldEvent);
               }
            }
         } else {
            client->events[oldEvent] = numReg;
         }
      }

      // Inform upstream client about new events
      if (!oldEvents.empty()) {
         if (m_upstreamClient) {
            informEvents(m_upstreamClient, oldEvents,
               tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CM_EVENTS);
         }
         if (m_downstreamManager) {
            informEvents(m_downstreamManager, oldEvents,
               tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CC_EVENTS);
         }
      }
   } else {
      for (std::vector<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
         uint32_t oldEvent = *it;

         m_log << tsd::common::logging::LogLevel::Trace
               << "deregisterClient <upstreamCM> " << oldEvent << std::endl;

         m_upstreamEvents.erase(oldEvent);
      }
   }
}

void CommunicationManager::handleRegisterDownstream(Buffer *msg)
{
   std::vector<uint32_t> events;
   extractEvents(msg, events);

   for (std::vector<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
      uint32_t newEvent = *it;

      m_log << tsd::common::logging::LogLevel::Trace
            << "registerDownstream " << newEvent << std::endl;

      m_downstreamEvents.insert(newEvent);
   }
}

void CommunicationManager::handleDeregisterDownstream(Buffer *msg)
{
   std::vector<uint32_t> events;
   extractEvents(msg, events);

   for (std::vector<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
      uint32_t oldEvent = *it;

      m_log << tsd::common::logging::LogLevel::Trace
            << "deregisterDownstream " << oldEvent << std::endl;

      m_downstreamEvents.erase(oldEvent);
   }
}

void CommunicationManager::handleUpstreamClient(Client *client)
{
   client->isManager = true;
   client->name = "upstreamCM";
   m_upstreamClient = client;

   // tell him all our registered events
   std::set<uint32_t> allEvents;
   for (std::map<uint32_t, std::set<Client*> >::iterator i = m_events.begin();
        i != m_events.end(); ++i) {
      allEvents.insert(i->first);
   }

   informEvents(client, allEvents, tsd::communication::event::TSDEVENTID_ADM_REGISTER_CM_EVENTS);
}

void CommunicationManager::handleHelo(Client *client, Buffer *msg)
{
   tsd::common::ipc::RpcBuffer buf;

   buf.init((char*)msg->payload(), msg->length());
   buf.getInt(); // discard event id
   buf >> client->name;

   // enable watchdog for this client, if not on black list
   bool enableWatchdog = true;
   for (std::vector<std::string>::const_iterator it(m_watchdogBlackList.begin());
        it != m_watchdogBlackList.end(); ++it) {
      if (client->name == *it) {
         enableWatchdog = false;
      }
   }

   if (enableWatchdog) {
      client->m_watchdogPending = 0;
   }
}

void CommunicationManager::handlePing(Client *client, Buffer *msg)
{
   tsd::common::ipc::RpcBuffer buf;
   uint32_t challenge;

   buf.init((char*)msg->payload(), msg->length());
   buf.getInt(); // discard event id
   buf >> challenge;

   // send back reply
   Buffer *replyBuf = allocBuffer(16);
   tsd::common::ipc::RpcBuffer reply;
   reply.init((char*)replyBuf->payload(), 16);

   reply << tsd::communication::event::TSDEVENTID_ADM_PONG;
   reply << ++challenge;
   assert(!reply.didOverflow());

   client->pushMessage(replyBuf);
   replyBuf->deref();
}

void CommunicationManager::handlePong(Client *client, Buffer *msg)
{
   tsd::common::ipc::RpcBuffer buf;
   uint32_t response;

   buf.init((char*)msg->payload(), msg->length());
   buf.getInt(); // discard event id
   buf >> response;

   if (response == client->m_watchdogChallenge + 1) {
      client->m_watchdogPending = 0;
   }
}

void CommunicationManager::handleWatchdog(Client *client, Buffer *msg)
{
   if (m_watchdogCallback && client->m_watchdogPending < WATCHDOG_TIMEOUT) {
      tsd::common::ipc::RpcBuffer buf;
      std::string queue;
      uint32_t event, timeEntered, timeStarted, timeExpired, now;

      buf.init((char*)msg->payload(), msg->length());
      buf.getInt(); // discard event id
      buf >> queue;
      buf >> event;
      buf >> timeEntered;
      buf >> timeStarted;
      buf >> timeExpired;
      buf >> now;

      // inhibit further watchdogs from this client
      client->m_watchdogPending = 0xfffe;
      client->m_watchdogChallenge++;

      m_watchdogCallback->comQueueWatchdog(client->name, client->pid, queue,
         event, timeEntered, timeStarted, timeExpired, now);
   }
}

void CommunicationManager::handleDisableRx(Buffer *msg)
{
   tsd::common::ipc::RpcBuffer buf;
   uint32_t state;
   std::string receiver;

   buf.init((char*)msg->payload(), msg->length());
   buf.getInt(); // discard event id
   buf >> state;
   buf >> receiver;

   for (std::list<Client*>::iterator client(m_clients.begin()); client != m_clients.end(); ++client) {
      if ((*client)->name == receiver) {
         (*client)->receiveEnabled = !!state;
         break;
      }
   }
}

void CommunicationManager::handleDisableTx(Buffer *msg)
{
   tsd::common::ipc::RpcBuffer buf;
   uint32_t state;
   std::string receiver;

   buf.init((char*)msg->payload(), msg->length());
   buf.getInt(); // discard event id
   buf >> state;
   buf >> receiver;

   for (std::list<Client*>::iterator client(m_clients.begin()); client != m_clients.end(); ++client) {
      if ((*client)->name == receiver) {
         (*client)->transmitEnabled = !!state;
         break;
      }
   }
}

void CommunicationManager::forwardMessage(Client *client, uint32_t event, Buffer *msg)
{
   std::map<uint32_t, std::set<Client*> >::iterator clientsIt = m_events.find(event);
   if (clientsIt != m_events.end()) {
      std::set<Client*> &clients = clientsIt->second;
      for (std::set<Client*>::iterator i = clients.begin(); i != clients.end(); ++i) {
         Client *dest = *i;
         if (dest->receiveEnabled) {
            dest->pushMessage(msg);
         }
      }
   }

   if (m_upstreamClient && !client->isManager) {
      std::set<uint32_t>::iterator upstreamIt = m_upstreamEvents.find(event);
      if (upstreamIt != m_upstreamEvents.end()) {
         m_upstreamClient->pushMessage(msg);
      }
   }

   if (m_downstreamManager && !client->isManager) {
      std::set<uint32_t>::iterator downstreamIt = m_downstreamEvents.find(event);
      if (downstreamIt != m_downstreamEvents.end()) {
         m_downstreamManager->pushMessage(msg);
      }
   }
}

void CommunicationManager::informEvents(Client *client, const std::set<uint32_t> &changedEvents, int32_t event)
{
   std::vector<uint32_t> ev;
   ev.reserve(changedEvents.size());
   for (std::set<uint32_t>::const_iterator i = changedEvents.begin(); i != changedEvents.end(); ++i) {
      ev.push_back(*i);
   }

   uint32_t bufLen = static_cast<uint32_t>(changedEvents.size()) * 5u + 16u;
   Buffer *buf = allocBuffer(bufLen);
   tsd::common::ipc::RpcBuffer rpc;
   rpc.init((char*)buf->payload(), bufLen);

   rpc.storeInt(event);
   rpc.storeString("bar");
   rpc << ev;
   assert(!rpc.didOverflow());

   client->pushMessage(buf);
   buf->deref();
}

void CommunicationManager::checkWatchdog()
{
   for (std::list<Client*>::iterator it(m_clients.begin()); it != m_clients.end(); ++it) {
      Client *client = *it;

      if (client->m_watchdogPending < WATCHDOG_TIMEOUT) {
         client->m_watchdogPending++;
         client->m_watchdogChallenge++;

         Buffer *requestBuf = allocBuffer(16);
         tsd::common::ipc::RpcBuffer request;
         request.init((char*)requestBuf->payload(), 16);

         request << tsd::communication::event::TSDEVENTID_ADM_PING;
         request << client->m_watchdogChallenge;
         assert(!request.didOverflow());

         client->pushMessage(requestBuf);
         requestBuf->deref();
      } else if (client->m_watchdogPending == WATCHDOG_TIMEOUT) {
         client->m_watchdogPending++;
         client->m_watchdogChallenge++; // prevent matching watchdog response to keep watchdog off
         m_watchdogCallback->comChannelDied(this, client->name, 0);
         m_watchdogCallback->comChannelDied(client->name, client->pid);
      }
   }
}

