/**
 * \file SendReceiveBackend.hpp
 * \brief QNX MsgSend/MsgReceive backend definition
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */
#ifndef __TCPBACKEND_HPP_
#define __TCPBACKEND_HPP_

#include <list>
#include <poll.h>
#include <queue>

#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/CommunicationManager.hpp>

namespace tsd { namespace communication {

class TcpClient;

class TcpBackend : public Backend,
                   public tsd::common::system::Thread
{
   public:
      TcpBackend(CommunicationManager &cm, tsd::common::logging::Logger &logger,
                 const std::string &address);
      virtual ~TcpBackend();

      virtual void run();

   private:
      void handleWakeup(short pollEvents);
      void handleConnect(short pollEvents);
      void handleDisconnect(TcpClient *client);
      void monitorWritable(TcpClient *client);
      void demonitorWritable(TcpClient *client);

      CommunicationManager &m_cm;
      tsd::common::logging::Logger &m_log;

      int m_wakeupPipe[2];
      int m_socket;
      struct pollfd m_fds[2];
      bool m_running;
      std::set<TcpClient*> m_clients;
      std::queue<TcpClient*> m_cleanupQueue;
      tsd::common::system::Mutex m_cleanupQueueLock;

      friend class TcpClient;
};

}}

#endif
