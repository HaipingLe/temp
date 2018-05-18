/**
 * \file tsd/communication/IComWatchdog.hpp
 * \brief ComMgr watchdog abstract interface definition
 * \author rene.reusner@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */

#ifndef TSD_COMMUNICATION_COMMMGR_WATCHDOG_HPP_INCLUDED
#define TSD_COMMUNICATION_COMMMGR_WATCHDOG_HPP_INCLUDED

#include <string>
#include <tsd/common/types/typedef.hpp>

namespace tsd { namespace communication {

class CommunicationManager;

class IComWatchdog {
public:
   virtual ~IComWatchdog();
   virtual void comChannelDied(CommunicationManager *mgr, const std::string &name, uint32_t event);
   virtual void comChannelDied(const std::string &name, int32_t pid);
   virtual void comQueueWatchdog(const std::string &name, int32_t pid,
      const std::string &queue, uint32_t event, uint32_t timeEntered,
      uint32_t timeStarted, uint32_t timeExpired, uint32_t now);
};



} } // tsd::communication

#endif
