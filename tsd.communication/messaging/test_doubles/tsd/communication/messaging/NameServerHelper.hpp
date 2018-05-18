//////////////////////////////////////////////////////////////////////
/// @file NameServerHelper.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Helper for NameServer
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_NAMESERVERHELPER_HPP
#define TSD_COMMUNICATION_MESSAGING_NAMESERVERHELPER_HPP

#include <gmock/gmock.h>
#include <tsd/communication/messaging/NameServer.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Helperclass for NameServer
 *
 * @brief Helperclass for NameServer
 */
class NameServerHelper : public NameServer
{
public:
   NameServerHelper(Router& router) : NameServer(router)
   {
   }
   ~NameServerHelper()
   {
   }
   using NameServer::isRunning;
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_NAMESERVERHELPER_HPP
