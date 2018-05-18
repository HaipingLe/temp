/**
 * \file tsd/communication/IComWatchdog.hpp
 * \brief ComMgr watchdog abstract interface implementation
 * \author rene.reusner@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */

#include <tsd/communication/IComWatchdog.hpp>

namespace tsd { namespace communication {


IComWatchdog::~IComWatchdog()
{
}

// Deprecated
void IComWatchdog::comChannelDied(CommunicationManager * /*mgr*/,
   const std::string &/*name*/, uint32_t /*event*/)
{
}

void IComWatchdog::comChannelDied(const std::string & /*name*/, int32_t /*pid*/)
{
}

void IComWatchdog::comQueueWatchdog(const std::string &/*name*/, int32_t /*pid*/,
      const std::string &/*queue*/, uint32_t /*event*/, uint32_t /*timeEntered*/,
      uint32_t /*timeStarted*/, uint32_t /*timeExpired*/, uint32_t /*now*/)
{
}


} } // tsd::communication

