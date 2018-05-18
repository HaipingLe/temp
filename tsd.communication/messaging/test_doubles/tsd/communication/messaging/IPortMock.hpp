//////////////////////////////////////////////////////////////////////
/// @file IPortMock.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Mock for IPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_IPORTMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_IPORTMOCK_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/IPort.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Mockclass for IPort
 *
 * @brief Mockclass for IPort
 */
class IPortMock : public IPort
{
public:
   IPortMock(Router& router) : IPort(router)
   {
   }
   // Mocked methods
   MOCK_METHOD0(startPort, bool());
   MOCK_METHOD0(stopPort, void());
   MOCK_METHOD0(sendPacketProxy, bool());

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
   bool sendPacket(std::auto_ptr<Packet> pkt)
   {
      UNUSED(pkt);
      return sendPacketProxy();
   }
#pragma GCC diagnostic pop
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_IPORTMOCK_HPP
