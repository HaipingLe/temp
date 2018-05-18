//////////////////////////////////////////////////////////////////////
/// @file ISelectEventHandlerMock.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Mock for ISelectEventHandler
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_ISELECTEVENTHANDLERMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_ISELECTEVENTHANDLERMOCK_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/Select.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Mockclass for ISelectEventHandler
 *
 * @brief Mockclass for ISelectEventHandler
 */
class ISelectEventHandlerMock : public ISelectEventHandler
{
public:
   // Mocked methods
   MOCK_METHOD0(selectReadable, bool());
   MOCK_METHOD0(selectWritable, bool());
   MOCK_METHOD0(selectError, bool());
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_ISELECTEVENTHANDLERMOCK_HPP
