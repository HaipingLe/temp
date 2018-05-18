//////////////////////////////////////////////////////////////////////
/// @file ConnectionHelper.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Helper for Connection
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTIONHELPER_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTIONHELPER_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/Connection.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Helperclass for Connection
 *
 * @brief Helperclass for Connection
 */
class ConnectionHelper : public Connection
{
public:
   ~ConnectionHelper()
   {
   }
   using Connection::setConnected;
   using Connection::setDisconnected;
   using Connection::processData;
   using Connection::getData;
   using Connection::consumeData;
   MOCK_METHOD0(setupConnection, bool());
   MOCK_METHOD0(teardownConnection, void());
   MOCK_METHOD0(wakeup, void());
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_CONNECTIONHELPER_HPP
