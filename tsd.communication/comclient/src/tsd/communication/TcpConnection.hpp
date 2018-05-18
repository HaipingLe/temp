///////////////////////////////////////////////////////
//!\file TcpConnection.hpp
//!\brief Declaration of the POSIX TCP connection
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_TCPCONNECTION_HPP
#define TSD_COMMUNICATION_TCPCONNECTION_HPP

#include <string>

#include <tsd/common/types/typedef.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/communication/Connection.hpp>

namespace tsd { namespace communication { namespace client {

class TcpRecvThread;
class TcpSendThread;

class TSD_COMMUNICATION_COMCLIENT_DLLEXPORT TcpConnection : public Connection
{
public:
   TcpConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log,
                 const std::string &name);
   TcpConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log,
                 int socket);
   virtual ~TcpConnection();

   virtual bool send(const void *buf, uint32_t len);
   virtual bool send(Buffer *buf);

   void disconnected();

private:
   TcpConnection(const TcpConnection&); // forbid copy ctor
   TcpConnection& operator=(const TcpConnection&); // forbid assignment operator

   int m_socket;
   TcpRecvThread *m_recvThread;
   TcpSendThread *m_sendThread;
   tsd::common::system::Mutex m_disconnectLock;

   friend class TcpSendThread;
   friend class TcpRecvThread;
};

}}}

#endif
