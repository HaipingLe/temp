/**
 * \file CommunicationManager.hpp
 * \brief Communication Manager definition
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */
#ifndef __COMMUNICATIONMANAGER_HPP_
#define __COMMUNICATIONMANAGER_HPP_

#include <list>
#include <set>
#include <string>
#include <vector>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/common/system/Semaphore.hpp>

#include <tsd/communication/IComWatchdog.hpp>


namespace tsd { namespace communication {

class Buffer;
class Client;

class Backend {
   public:
      virtual ~Backend();
};

class TSD_COMMUNICATION_COMMGR_DLLEXPORT CommunicationManager
   : protected tsd::common::system::Thread
{
   public:
      CommunicationManager();
      ~CommunicationManager();

      enum {
         WATCHDOG_TIMEOUT = 30   /* watchdog timeout in seconds */
      };

      void addBackend(const std::string &address);
      void connectDownstreamCM(const std::string &address);
      void registerTimeOutHandler(IComWatchdog *callback);
      void setDebug(bool enable);

      void setWatchdogBlacklist(const std::vector<std::string> &blackList);

      // The following methods are ComMgr internal

      void registerClient(Client *client);
      void deregisterClient(Client *client);
      void dispatchMessage(Client *client, Buffer *buf);

      inline void lock()
      {
         m_lock.lock();
      }

      inline void unlock()
      {
         m_lock.unlock();
      }

   protected:
      void run(); // tsd::common::system::Thread

   private:
      void handleRegisterClient(Client *client, Buffer *msg);
      void handleDeregisterClient(Client *client, Buffer *msg);
      void handleRegisterDownstream(Buffer *msg);
      void handleDeregisterDownstream(Buffer *msg);
      void handleUpstreamClient(Client *client);
      void handleHelo(Client *client, Buffer *msg);
      void handlePing(Client *client, Buffer *msg);
      void handlePong(Client *client, Buffer *msg);
      void handleWatchdog(Client *client, Buffer *msg);
      void handleDisableRx(Buffer *msg);
      void handleDisableTx(Buffer *msg);
      void forwardMessage(Client *client, uint32_t event, Buffer *msg);
      void informEvents(Client *client, const std::set<uint32_t> &changedEvents, int32_t event);
      void checkWatchdog();

      tsd::common::logging::Logger m_log;
      std::list<Backend*> m_backends;
      std::list<Client*> m_clients;
      std::map<uint32_t, std::set<Client*> > m_events;
      std::set<uint32_t> m_upstreamEvents;
      std::set<uint32_t> m_downstreamEvents;
      Client *m_upstreamClient;
      Client *m_downstreamManager;
      tsd::common::system::Mutex m_lock;

      IComWatchdog *m_watchdogCallback;
      tsd::common::system::Semaphore m_watchdogSemaphore;
      std::vector<std::string> m_watchdogBlackList;
};

} /* namespace communication */ } /* namespace tsd */

#endif
