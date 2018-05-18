//////////////////////////////////////////////////////////////////////
/// @file IMessageFactoryMock.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Mock for IMessageFactory
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_IMESSAGEFACTORYMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_IMESSAGEFACTORYMOCK_HPP

#include <gmock/gmock.h>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/IMessageFactory.hpp>
#include <vector>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Mockclass for IMessageFactory
 *
 * @brief Mockclass for IMessageFactory
 */
class IMessageFactoryMock : public IMessageFactory
{
public:
   // Mocked methods
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
   MOCK_CONST_METHOD1(createEvent, std::auto_ptr<tsd::communication::event::TsdEvent>(uint32_t msgId));
#pragma GCC diagnostic pop
   MOCK_CONST_METHOD0(getEventIds, std::vector<uint32_t>());
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_IMESSAGEFACTORYMOCK_HPP
