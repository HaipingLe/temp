#ifndef TSD_COMMUNICATION_MESSAGING_TCPCLIENTPORT_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPCLIENTPORT_HPP

#include <netinet/in.h>

#include <tsd/communication/messaging/Connection.hpp>

namespace tsd { namespace communication { namespace messaging {

class TcpClientPort
   : public IConnection
{
   class Impl;
   Impl *m_p;

public:
   TcpClientPort(Router &router, const std::string &subDomain = "");
   ~TcpClientPort();

   void disconnect(); // IConnection

   void initV4(uint32_t addr = INADDR_LOOPBACK, uint16_t port = 24710,
               uint32_t sndBuf = 0, uint32_t rcvBuf = 0);

   void initUnix(const std::string &path);
};

} } }

#endif
