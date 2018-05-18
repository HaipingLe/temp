//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! This file contains the class implementions of all serializeable events
//! for the inter process communication
//!
//! \file    tsd/communication/event/TsdEvent.cpp
//! \author  Oliver Flatau, Stefan Zill
//! \brief   Definition of TsdEvent class
//!
//////////////////////////////////////////////////////////////////////

#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

//! Destructor
TsdEvent::~TsdEvent()
{
}

//! virtual function to serialize object data to given buffer
//! @param[in] buf Buffer to store serializes data
void TsdEvent::serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const
{
   //buf << m_EventId;
}

//! virtual function to deserialize object data from given buffer
//! @param[out] buf Buffer with serialized data
void TsdEvent::deserialize(tsd::common::ipc::RpcBuffer& /*buf*/)
{
   //buf >> m_EventId;
}

//! virtual function to make a deep copy of the object
//! @return clone of the object
TsdEvent* TsdEvent::clone(void) const
{
   return new TsdEvent(getEventId());
}

tsd::common::ipc::RpcBuffer& operator<<(tsd::common::ipc::RpcBuffer& buffer, const TsdEvent& event)
{
   event.serialize(buffer);
   return buffer;
}

tsd::common::ipc::RpcBuffer& operator>>(tsd::common::ipc::RpcBuffer& buffer, TsdEvent& event)
{
   event.deserialize(buffer);
   return buffer;
}

} /* namespace event */ } /* namespace communication */ } /* namespace tsd */
