//////////////////////////////////////////////////////////////////////
//!\file TcpConnectionWindows.hpp
//!\brief TODO: definition of file TcpConnectionWindows.hpp
//!\author d.wassenberg@technisat.de
//!
//!Copyright (c) TechniSat Digital GmbH
//!CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_TCPCONNECTIONWINDOWS_HPP
#define TSD_COMMUNICATION_TCPCONNECTIONWINDOWS_HPP

#include <string>

#include <tsd/common/types/typedef.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/communication/Connection.hpp>
#include <tsd/communication/Buffer.hpp>

#include <winsock2.h>
#include <windows.h>

namespace tsd {
namespace communication {
namespace client {

class TcpRecvThread;
class TcpSendThread;

class TSD_COMMUNICATION_COMCLIENT_DLLEXPORT TcpConnection : public Connection {
public:
   TcpConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log, const std::string &name);
   TcpConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log, SOCKET socket);
   virtual ~TcpConnection();

   virtual bool send(const void *buf, uint32_t len);
   virtual bool send(tsd::communication::Buffer *buf);

   void disconnected();

private:
   SOCKET m_socket;
   TcpRecvThread *m_recvThread;
   TcpSendThread *m_sendThread;
   tsd::common::system::Mutex m_disconnectLock;

   friend class TcpSendThread;
   friend class TcpRecvThread;
};

}
}
}

#endif // TSD_COMMUNICATION_TCPCONNECTIONWINDOWS_HPP
