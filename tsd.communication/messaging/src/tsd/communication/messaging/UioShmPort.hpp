#ifndef TSD_COMMUNICATION_MESSAGING_UIOSHMPORT_HPP
#define TSD_COMMUNICATION_MESSAGING_UIOSHMPORT_HPP

#include <string>

#include <tsd/communication/messaging/Connection.hpp>

namespace tsd { namespace communication { namespace messaging {

class Router;

class UioShmPort
   : public IConnection
{
   class Listener;
   class Connector;

   Router &m_router;
   Listener *m_listener;
   Connector *m_connector;

public:
   UioShmPort(Router &router);
   ~UioShmPort();

   void connectUpstream(const std::string &url, const std::string &subDomain);
   void listenDownstream(const std::string &url);

   void disconnect(); // IConnection
};

} } }

#endif
