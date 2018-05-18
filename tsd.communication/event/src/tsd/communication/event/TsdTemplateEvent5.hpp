//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/TsdTemplateEvent5.cpp
//! \author  Stefan Zill
//! \brief   TsdEvent carrying 5 data elements
//!
//////////////////////////////////////////////////////////////////////

#ifndef TSDTEMPLATEEVENT5_HPP_
#define TSDTEMPLATEEVENT5_HPP_

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

template <class T1, class T2, class T3, class T4, class T5>
class TsdTemplateEvent5 : public ::tsd::communication::event::TsdEvent
{
public:
   typedef T1  type1;
   typedef T2  type2;
   typedef T3  type3;
   typedef T4  type4;
   typedef T5  type5;

   explicit TsdTemplateEvent5(uint32_t eventid);
   explicit TsdTemplateEvent5(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5);

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

   const T3& getData3() const { return m_Data3; }
   void setData3(const T3& data3) { m_Data3 = data3; }

   const T4& getData4() const { return m_Data4; }
   void setData4(const T4& data4) { m_Data4 = data4; }

   const T5& getData5() const { return m_Data5; }
   void setData5(const T5& data5) { m_Data5 = data5; }

private:
   T1  m_Data1;
   T2  m_Data2;
   T3  m_Data3;
   T4  m_Data4;
   T5  m_Data5;
};

template <class T1, class T2, class T3, class T4, class T5>
TsdTemplateEvent5<T1, T2, T3, T4, T5>::TsdTemplateEvent5(uint32_t eventid)
: TsdEvent(eventid)
, m_Data1()
, m_Data2()
, m_Data3()
, m_Data4()
, m_Data5()
{
}

template <class T1, class T2, class T3, class T4, class T5>
TsdTemplateEvent5<T1, T2, T3, T4, T5>::TsdTemplateEvent5(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5)
: TsdEvent(eventid)
, m_Data1(data1)
, m_Data2(data2)
, m_Data3(data3)
, m_Data4(data4)
, m_Data5(data5)
{
}

template <class T1, class T2, class T3, class T4, class T5>
void TsdTemplateEvent5<T1, T2, T3, T4, T5>::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   //TsdEvent::serialize(buf);
   buf << m_Data1;
   buf << m_Data2;
   buf << m_Data3;
   buf << m_Data4;
   buf << m_Data5;
}

template <class T1, class T2, class T3, class T4, class T5>
void TsdTemplateEvent5<T1, T2, T3, T4, T5>::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   //TsdEvent::deserialize(buf);
   buf >> m_Data1;
   buf >> m_Data2;
   buf >> m_Data3;
   buf >> m_Data4;
   buf >> m_Data5;
}

template <class T1, class T2, class T3, class T4, class T5>
TsdEvent* TsdTemplateEvent5<T1, T2, T3, T4, T5>::clone(void) const
{
   return new TsdTemplateEvent5<T1, T2, T3, T4, T5>(getEventId(), m_Data1, m_Data2, m_Data3, m_Data4, m_Data5);
}

} /* namespace event */ } /* namespace communication */ } /* namespace tsd */

#endif /* TSDTEMPLATEEVENT5_HPP_ */
