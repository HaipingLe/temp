#ifndef TSD_COMMUNICATION_MESSAGING_TCPSERVERPORT_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPSERVERPORT_HPP

#include <netinet/in.h>

#include <tsd/communication/messaging/Connection.hpp>

namespace tsd { namespace communication { namespace messaging {

class TcpServerPort
   : public IConnection
{
   class Impl;
   Impl *m_p;

public:
   TcpServerPort(Router &router);
   ~TcpServerPort();

   void disconnect(); // IConnection

   void initV4(uint32_t addr = INADDR_ANY, uint16_t port = 24710,
               uint32_t sndBuf = 0, uint32_t rcvBuf = 0);

   void initUnix(const std::string &path);

   uint16_t getBoundPort();
   std::string getBoundPath();
};

} } }

#endif
