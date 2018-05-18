//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/TsdTemplateEvent11.cpp
//! \author  Carla Schreiber
//! \brief   TsdEvent carrying 11 data elements
//! \date    07.10.2011
//!
//////////////////////////////////////////////////////////////////////

#ifndef TSDTEMPLATEEVENT11_HPP_
#define TSDTEMPLATEEVENT11_HPP_

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
class TsdTemplateEvent11 : public ::tsd::communication::event::TsdEvent
{
public:
/*   typedef T1  type1;
   typedef T2  type2;
   typedef T3  type3;
   typedef T4  type4;
   typedef T5  type5;
   typedef T6  type6;
   typedef T7  type7;
   typedef T8  type8;
   typedef T9  type9;
   typedef T10 type10;
   typedef T11 type11;
*/
   //! explicit Constructor
   //! @param[in] eventid the event id
   explicit TsdTemplateEvent11(uint32_t eventid);

   //! explicit Constructor with parameters for each data
   //! @param[in] eventid the event id
   //! @param[in] data1 the 1. data
   //! @param[in] data2 the 2. data
   //! @param[in] data3 the 3. data
   //! @param[in] data4 the 4. data
   //! @param[in] data5 the 5. data
   //! @param[in] data6 the 6. data
   //! @param[in] data7 the 7. data
   //! @param[in] data8 the 8. data
   //! @param[in] data9 the 9. data
   //! @param[in] data10 the 10. data
   //! @param[in] data11 the 11. data
   explicit TsdTemplateEvent11(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const T6& data6, const T7& data7, const T8& data8, const T9& data9, const T10& data10, const T11& data11);

   //! virtual function to serialize object data to given buffer
   //! @param[in] buf Buffer to store serializes data
   virtual void serialize(tsd::common::ipc::RpcBuffer& buf) const;

   //! virtual function to deserialize object data from given buffer
   //! @param[out] buf Buffer with serialized data
   virtual void deserialize(tsd::common::ipc::RpcBuffer& buf);

   //! create an exact duplicate of the current event
   virtual TsdEvent* clone(void) const;

    //! getter - for 1. data
   const T1& getData1() const { return m_Data1; }
   //! setter - for 1. data
   void setData1(const T1& data1) { m_Data1 = data1; }

   //! getter - for 2. data
   const T2& getData2() const { return m_Data2; }
   //! setter - for 2. data
   void setData2(const T2& data2) { m_Data2 = data2; }

   //! getter - for 3. data
   const T3& getData3() const { return m_Data3; }
   //! setter - for 3. data
   void setData3(const T3& data3) { m_Data3 = data3; }

   //! getter - for 4. data
   const T4& getData4() const { return m_Data4; }
  //! setter - for 4. data 
   void setData4(const T4& data4) { m_Data4 = data4; }

	//! getter - for 5. data
   const T5& getData5() const { return m_Data5; }
   //! setter - for 5. data
   void setData5(const T5& data5) { m_Data5 = data5; }

   //! getter - for 6. data
   const T6& getData6() const { return m_Data6; }
   //! setter - for 6. data
   void setData6(const T6& data6) { m_Data6 = data6; }

   //! getter - for 7. data
   const T7& getData7() const { return m_Data7; }
   //! setter - for 7. data
   void setData7(const T7& data7) { m_Data7 = data7; }

   //! getter - for 8. data
   const T8& getData8() const { return m_Data8; }
   //! setter - for 8. data
   void setData8(const T8& data8) { m_Data8 = data8; }

   //! getter - for 9. data
   const T9& getData9() const { return m_Data9; }
   //! setter - for 9. data
   void setData9(const T9& data9) { m_Data9 = data9; }

   //! getter - for 10. data
   const T10& getData10() const { return m_Data10; }
   //! setter - for 10. data
   void setData10(const T10& data10) { m_Data10 = data10; }

   //! getter - for 11. data 
   const T11& getData11() const { return m_Data11; }
   //! setter - for 11. data
   void setData11(const T11& data11) { m_Data11 = data11; }

private:
   T1  m_Data1;
   T2  m_Data2;
   T3  m_Data3;
   T4  m_Data4;
   T5  m_Data5;
   T6  m_Data6;
   T7  m_Data7;
   T8  m_Data8;
   T9  m_Data9;
   T10 m_Data10;
   T11 m_Data11;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
TsdTemplateEvent11<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::TsdTemplateEvent11(uint32_t eventid)
: TsdEvent(eventid)
, m_Data1()
, m_Data2()
, m_Data3()
, m_Data4()
, m_Data5()
, m_Data6()
, m_Data7()
, m_Data8()
, m_Data9()
, m_Data10()
, m_Data11()
{
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
TsdTemplateEvent11<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::TsdTemplateEvent11(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const T6& data6, const T7& data7, const T8& data8, const T9& data9, const T10& data10, const T11& data11)
: TsdEvent(eventid)
, m_Data1(data1)
, m_Data2(data2)
, m_Data3(data3)
, m_Data4(data4)
, m_Data5(data5)
, m_Data6(data6)
, m_Data7(data7)
, m_Data8(data8)
, m_Data9(data9)
, m_Data10(data10)
, m_Data11(data11)
{
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
void TsdTemplateEvent11<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   buf << m_Data1;
   buf << m_Data2;
   buf << m_Data3;
   buf << m_Data4;
   buf << m_Data5;
   buf << m_Data6;
   buf << m_Data7;
   buf << m_Data8;
   buf << m_Data9;
   buf << m_Data10;
   buf << m_Data11;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
void TsdTemplateEvent11<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   buf >> m_Data1;
   buf >> m_Data2;
   buf >> m_Data3;
   buf >> m_Data4;
   buf >> m_Data5;
   buf >> m_Data6;
   buf >> m_Data7;
   buf >> m_Data8;
   buf >> m_Data9;
   buf >> m_Data10;
   buf >> m_Data11;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
TsdEvent* TsdTemplateEvent11<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::clone(void) const
{
   return new TsdTemplateEvent11<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>(getEventId(), m_Data1, m_Data2, m_Data3, m_Data4, m_Data5, m_Data6, m_Data7, m_Data8, m_Data9, m_Data10, m_Data11);
}

} /* namespace event */ } /* namespace communication */ } /* namespace tsd */

#endif /* TSDTEMPLATEEVENT11_HPP_ */
