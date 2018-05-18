//////////////////////////////////////////////////////////////////////
/// @file TcpEndpointMock.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Mock for TcpEndpoint
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_TCPENDPOINTMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPENDPOINTMOCK_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/TcpEndpoint.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Mockclass for TcpEndpoint
 *
 * @brief Mockclass for TcpEndpoint
 */
class TcpEndpointMock : public TcpEndpoint
{
public:
   TcpEndpointMock(tsd::common::logging::Logger& log) : TcpEndpoint(log)
   {
   }
   using TcpEndpoint::epSendPacket;

   MOCK_METHOD0(epReceivedPacketProxy, void());
   MOCK_METHOD0(epDisconnected, void());

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
   void                epReceivedPacket(std::auto_ptr<Packet>)
   {
      epReceivedPacketProxy();
   }
#pragma GCC diagnostic pop
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_TCPENDPOINTMOCK_HPP
