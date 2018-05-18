//////////////////////////////////////////////////////////////////////
//!\file TcpBackendWindows.hpp
//!\brief TODO: definition of file TcpBackendWindows.hpp
//!\author d.wassenberg@technisat.de
//!
//!Copyright (c) TechniSat Digital GmbH
//!CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TCPBACKENDWINDOWS_HPP_
#define TCPBACKENDWINDOWS_HPP_

#include <list>
#include <queue>

#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/CommunicationManager.hpp>

#include <winsock2.h>
#include <windows.h>

namespace tsd {
namespace communication {

class TcpClient;
class TcpBackendListener;

class TcpBackend : public Backend,
                   public tsd::common::system::Thread {
public:
   TcpBackend(CommunicationManager &cm, tsd::common::logging::Logger &logger, const std::string &address);
   virtual ~TcpBackend();

   virtual void run();

private:
   void handleConnect(SOCKET socket);
   void handleDisconnect(TcpClient *client);
   void monitorWritable(TcpClient *client);
   void demonitorWritable(TcpClient *client);

   CommunicationManager &m_cm;
   tsd::common::logging::Logger &m_log;

   TcpBackendListener* m_Listener;

   bool m_running;
   std::set<TcpClient*> m_clients;
   std::queue<TcpClient*> m_cleanupQueue;
   tsd::common::system::Mutex m_cleanupQueueLock;
   tsd::common::system::Semaphore m_WaitSema;

   friend class TcpClient;
   friend class TcpBackendListener;
};

}
}

#endif /* TCPBACKENDWINDOWS_HPP_ */
