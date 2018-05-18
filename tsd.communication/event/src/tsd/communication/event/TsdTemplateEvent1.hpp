//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/TsdTemplateEvent1.cpp
//! \author  Stefan Zill
//! \brief   TsdEvent carrying 1 data element
//!
//////////////////////////////////////////////////////////////////////

#ifndef TSDTEMPLATEEVENT1_HPP_
#define TSDTEMPLATEEVENT1_HPP_

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

template <class T1>
class TsdTemplateEvent1 : public ::tsd::communication::event::TsdEvent
{
public:
   typedef T1 type1;

   explicit TsdTemplateEvent1(uint32_t eventid);
   explicit TsdTemplateEvent1(uint32_t eventid, const T1& data1);

   //! virtual function to serialize object data to given buffer
   //! @param[in] buf Buffer to store serializes data
   virtual void serialize(tsd::common::ipc::RpcBuffer& buf) const;

   //! virtual function to deserialize object data from given buffer
   //! @param[out] buf Buffer with serialized data
   virtual void deserialize(tsd::common::ipc::RpcBuffer& buf);

   //! create an exact duplicate of the current event
   virtual TsdEvent* clone(void) const;

   const T1& getData1() const { return m_Data1; }
   void setData1(const T1& data1) { m_Data1 = data1; }

private:
   T1 m_Data1;
};

template <class T1>
TsdTemplateEvent1<T1>::TsdTemplateEvent1(uint32_t eventid)
: TsdEvent(eventid)
, m_Data1()
{
}

template <class T1>
TsdTemplateEvent1<T1>::TsdTemplateEvent1(uint32_t eventid, const T1& data1)
: TsdEvent(eventid)
, m_Data1(data1)
{
}

template <class T1>
void TsdTemplateEvent1<T1>::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   //TsdEvent::serialize(buf);
   buf << m_Data1;
}

template <class T1>
void TsdTemplateEvent1<T1>::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   //TsdEvent::deserialize(buf);
   buf >> m_Data1;
}

template <class T1>
TsdEvent* TsdTemplateEvent1<T1>::clone(void) const
{
   return new TsdTemplateEvent1<T1>(getEventId(), m_Data1);
}

} /* namespace events */ } /* namespace communication */ } /* namespace tsd */

#endif /* TSDTEMPLATEEVENT1_HPP_ */
