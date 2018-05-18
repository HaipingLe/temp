//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/TsdTemplateEvent2.cpp
//! \author  Stefan Zill
//! \brief   TsdEvent carrying 2 data elements
//!
//////////////////////////////////////////////////////////////////////

#ifndef TSDTEMPLATEEVENT2_HPP_
#define TSDTEMPLATEEVENT2_HPP_

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

template <class T1, class T2>
class TsdTemplateEvent2 : public ::tsd::communication::event::TsdEvent
{
public:
   typedef T1 type1;
   typedef T2 type2;

   explicit TsdTemplateEvent2(uint32_t eventid);
   explicit TsdTemplateEvent2(uint32_t eventid, const T1& data1, const T2& data2);

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

   const T2& getData2() const { return m_Data2; }
   void setData2(const T2& data2) { m_Data2 = data2; }

private:
   T1 m_Data1;
   T2 m_Data2;
};

template <class T1, class T2>
TsdTemplateEvent2<T1,T2>::TsdTemplateEvent2(uint32_t eventid)
: TsdEvent(eventid)
, m_Data1()
, m_Data2()
{
}

template <class T1, class T2>
TsdTemplateEvent2<T1,T2>::TsdTemplateEvent2(uint32_t eventid, const T1& data1, const T2& data2)
: TsdEvent(eventid)
, m_Data1(data1)
, m_Data2(data2)
{
}

template <class T1, class T2>
void TsdTemplateEvent2<T1,T2>::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   //TsdEvent::serialize(buf);
   buf << m_Data1;
   buf << m_Data2;
}

template <class T1, class T2>
void TsdTemplateEvent2<T1,T2>::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   //TsdEvent::deserialize(buf);
   buf >> m_Data1;
   buf >> m_Data2;
}

template <class T1, class T2>
TsdEvent* TsdTemplateEvent2<T1, T2>::clone(void) const
{
   return new TsdTemplateEvent2<T1, T2>(getEventId(), m_Data1, m_Data2);
}

} /* namespace event */ } /* namespace communication */ } /* namespace tsd */

#endif /* TSDTEMPLATEEVENT2_HPP_ */
