//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/TsdTemplateEvent6.cpp
//! \author  Stefan Zill
//! \brief   TsdEvent carrying 6 data elements
//!
//////////////////////////////////////////////////////////////////////

#ifndef TSDTEMPLATEEVENT6_HPP_
#define TSDTEMPLATEEVENT6_HPP_

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

template <class T1, class T2, class T3, class T4, class T5, class T6>
class TsdTemplateEvent6 : public ::tsd::communication::event::TsdEvent
{
public:
   typedef T1  type1;
   typedef T2  type2;
   typedef T3  type3;
   typedef T4  type4;
   typedef T5  type5;
   typedef T6  type6;

   explicit TsdTemplateEvent6(uint32_t eventid);
   explicit TsdTemplateEvent6(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const T6& data6);

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

   const T6& getData6() const { return m_Data6; }
   void setData6(const T6& data6) { m_Data6 = data6; }

private:
   T1  m_Data1;
   T2  m_Data2;
   T3  m_Data3;
   T4  m_Data4;
   T5  m_Data5;
   T6  m_Data6;
};

template <class T1, class T2, class T3, class T4, class T5, class T6>
TsdTemplateEvent6<T1, T2, T3, T4, T5, T6>::TsdTemplateEvent6(uint32_t eventid)
: TsdEvent(eventid)
, m_Data1()
, m_Data2()
, m_Data3()
, m_Data4()
, m_Data5()
, m_Data6()
{
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
TsdTemplateEvent6<T1, T2, T3, T4, T5, T6>::TsdTemplateEvent6(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const T6& data6)
: TsdEvent(eventid)
, m_Data1(data1)
, m_Data2(data2)
, m_Data3(data3)
, m_Data4(data4)
, m_Data5(data5)
, m_Data6(data6)
{
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
void TsdTemplateEvent6<T1, T2, T3, T4, T5, T6>::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   //TsdEvent::serialize(buf);
   buf << m_Data1;
   buf << m_Data2;
   buf << m_Data3;
   buf << m_Data4;
   buf << m_Data5;
   buf << m_Data6;
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
void TsdTemplateEvent6<T1, T2, T3, T4, T5, T6>::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   //TsdEvent::deserialize(buf);
   buf >> m_Data1;
   buf >> m_Data2;
   buf >> m_Data3;
   buf >> m_Data4;
   buf >> m_Data5;
   buf >> m_Data6;
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
TsdEvent* TsdTemplateEvent6<T1, T2, T3, T4, T5, T6>::clone(void) const
{
   return new TsdTemplateEvent6<T1, T2, T3, T4, T5, T6>(getEventId(), m_Data1, m_Data2, m_Data3, m_Data4, m_Data5, m_Data6);
}

} /* namespace event */ } /* namespace communication */ } /* namespace tsd */

#endif /* TSDTEMPLATEEVENT6_HPP_ */
