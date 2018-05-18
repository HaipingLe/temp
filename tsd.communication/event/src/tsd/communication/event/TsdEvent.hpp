//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! This file contains the class definitions of all serializeable events
//! for the inter process communication
//!
//! \file    tsd/communication/event/TsdEvent.hpp
//! \author  Oliver Flatau, Stefan Zill
//! \brief   Declaration of TsdEvent class
//!
//////////////////////////////////////////////////////////////////////
#ifndef _TSDEVENT_HPP_
#define _TSDEVENT_HPP_

#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/common/types/typedef.hpp>

namespace tsd { namespace communication { namespace event {

typedef uint64_t IfcAddr_t;

static const IfcAddr_t LOOPBACK_ADDRESS = IfcAddr_t(0);

//////////////////////////////////////////////////////////////////////
//! \class  TsdEvent
//! \author Oliver Flatau
//! \date   2011/03/15
//! \brief  Definition of the class TsdEvent
//!
//! TsdEvent defines the abstract base class for all serializeable events
//! for the inter process communication.
//!
//////////////////////////////////////////////////////////////////////
class TSD_COMMUNICATION_EVENT_DLLEXPORT TsdEvent
{
public:

   //! Constructor to initialize with given event identifier
   //! @param[in] eventid event identifier
   explicit TsdEvent(uint32_t eventid)
      : m_EventId(eventid)
      , m_sender(LOOPBACK_ADDRESS)
      , m_receiver(LOOPBACK_ADDRESS)
   { }

   //! Destructor
   virtual ~TsdEvent(void);

  //! virtual function to serialize object data to given buffer
   //! @param[in] buf Buffer to store serializes data
   virtual void serialize(tsd::common::ipc::RpcBuffer& buf) const;

   //! virtual function to deserialize object data from given buffer
   //! @param[out] buf Buffer with serialized data
   virtual void deserialize(tsd::common::ipc::RpcBuffer& buf);

   //! virtual function to make a deep copy of the object
   //! @return clone of the object
   virtual TsdEvent* clone(void) const;

   //! getter -
   uint32_t getEventId() const { return m_EventId; }

   inline IfcAddr_t getSenderAddr() const { return m_sender; }
   inline void setSenderAddr(IfcAddr_t sender) { m_sender = sender; }
   inline IfcAddr_t getReceiverAddr() const { return m_receiver; }
   inline void setReceiverAddr(IfcAddr_t receiver) { m_receiver = receiver; }

private:
   uint32_t m_EventId;  //!< Identfier of the event
   IfcAddr_t m_sender;
   IfcAddr_t m_receiver;

   //! forbidden assignment operator
   TsdEvent& operator=(const TsdEvent&);

   //! forbidden copy constructor
   TsdEvent(const TsdEvent&);
};
// streaming operators
TSD_COMMUNICATION_EVENT_DLLEXPORT tsd::common::ipc::RpcBuffer& operator<<(tsd::common::ipc::RpcBuffer& buffer, const tsd::communication::event::TsdEvent& event);
TSD_COMMUNICATION_EVENT_DLLEXPORT tsd::common::ipc::RpcBuffer& operator>>(tsd::common::ipc::RpcBuffer& buffer, tsd::communication::event::TsdEvent& event);

} /* namespace event */} /* namespace communication */ } /* namespace tsd */


#endif // _TSDEVENT_HPP_
