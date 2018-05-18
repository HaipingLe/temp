//////////////////////////////////////////////////////////////////////
/// @file IIfcNotifiyMock.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Mock for IIfcNotifiy
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_IIFCNOTIFIYMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_IIFCNOTIFIYMOCK_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/Queue.cpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Mockclass for IIfcNotifiy
 *
 * @brief Mockclass for IIfcNotifiy
 */
class IIfcNotifiyMock : public IIfcNotifiy
{
public:
   // Mocked methods
   MOCK_METHOD1(notifyDead, void(tsd::communication::event::IfcAddr_t sender));
   MOCK_METHOD1(isSubscribed, bool(uint32_t event));
   MOCK_METHOD0(getName, std::string(void));
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_IIFCNOTIFIYMOCK_HPP
