#ifndef TSD_COMMUNICATION_MESSAGING_INTERNALPORT_HPP
#define TSD_COMMUNICATION_MESSAGING_INTERNALPORT_HPP

#include <tsd/communication/messaging/IPort.hpp>

namespace tsd { namespace communication { namespace messaging {

class Router;
class InternalPortBridge;

class InternalPort
{
   InternalPortBridge *m_bridge;

public:
   InternalPort();
   ~InternalPort();

   bool connect(Router &upstreamRouter, Router &downstreamRouter,
                const std::string &subDomain = "");

   bool isIdle();
};

} } }

#endif
