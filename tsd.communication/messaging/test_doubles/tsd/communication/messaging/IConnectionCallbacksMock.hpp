//////////////////////////////////////////////////////////////////////
/// @file IConnectionCallbacksMock.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Mock for IConnectionCallbacks
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_ICONNECTIONCALLBACKSMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_ICONNECTIONCALLBACKSMOCK_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/Connection.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Mockclass for IConnectionCallbacks
 *
 * @brief Mockclass for IConnectionCallbacks
 */
class IConnectionCallbacksMock : public IConnectionCallbacks
{
public:
   // Mocked methods
   MOCK_METHOD0(setupConnection, bool());
   MOCK_METHOD0(teardownConnection, void());
   MOCK_METHOD0(wakeup, void());
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_ICONNECTIONCALLBACKSMOCK_HPP
