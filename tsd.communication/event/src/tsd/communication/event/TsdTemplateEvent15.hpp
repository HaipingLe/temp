//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2012
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/TsdTemplateEvent15.hpp
//! \author  Stefan Zill
//! \brief   TsdEvent carrying 15 data elements
//! \date    23.07.2012
//!
//////////////////////////////////////////////////////////////////////

#ifndef TSDTEMPLATEEVENT15_HPP_
#define TSDTEMPLATEEVENT15_HPP_

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace event {

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
class TsdTemplateEvent15 : public ::tsd::communication::event::TsdEvent
{
public:
   //! explicit Constructor
   //! @param[in] eventid the event id
   explicit TsdTemplateEvent15(uint32_t eventid);

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
   //! @param[in] data12 the 12. data
   //! @param[in] data13 the 13. data
   //! @param[in] data14 the 14. data
   //! @param[in] data14 the 15. data
   explicit TsdTemplateEvent15(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const T6& data6, const T7& data7, const T8& data8, const T9& data9, const T10& data10, const T11& data11, const T12& data12, const T13& data13, const T14& data14, const T15& data15 );

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

   //! getter - for 12. data 
   const T12& getData12() const { return m_Data12; }
   //! setter - for 12. data
   void setData12(const T12& data12) { m_Data12 = data12; }

   //! getter - for 13. data 
   const T13& getData13() const { return m_Data13; }
   //! setter - for 13. data
   void setData13(const T13& data13) { m_Data13 = data13; }

   //! getter - for 14. data 
   const T14& getData14() const { return m_Data14; }
   //! setter - for 14. data
   void setData14(const T14& data14) { m_Data14 = data14; }
   
   //! getter - for 15. data 
   const T15& getData15() const { return m_Data15; }
   //! setter - for 15. data
   void setData15(const T15& data15) { m_Data15 = data15; }

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
   T12 m_Data12;
   T13 m_Data13;
   T14 m_Data14;
   T15 m_Data15;
};

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
TsdTemplateEvent15<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::TsdTemplateEvent15(uint32_t eventid)
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
, m_Data12()
, m_Data13()
, m_Data14()
, m_Data15()
{
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
TsdTemplateEvent15<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::TsdTemplateEvent15(uint32_t eventid, const T1& data1, const T2& data2, const T3& data3, const T4& data4, const T5& data5, const T6& data6, const T7& data7, const T8& data8, const T9& data9, const T10& data10, const T11& data11, const T12& data12, const T13& data13, const T14& data14, const T15& data15)
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
, m_Data12(data12)
, m_Data13(data13)
, m_Data14(data14)
, m_Data15(data15)
{
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
void TsdTemplateEvent15<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::serialize(tsd::common::ipc::RpcBuffer& buf) const
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
   buf << m_Data12;
   buf << m_Data13;
   buf << m_Data14;
   buf << m_Data15;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
void TsdTemplateEvent15<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::deserialize(tsd::common::ipc::RpcBuffer& buf)
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
   buf >> m_Data12;
   buf >> m_Data13;
   buf >> m_Data14;
   buf >> m_Data15;
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
TsdEvent* TsdTemplateEvent15<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::clone(void) const
{
   return new TsdTemplateEvent15<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>(getEventId(), m_Data1, m_Data2, m_Data3, m_Data4, m_Data5, m_Data6, m_Data7, m_Data8, m_Data9, m_Data10, m_Data11, m_Data12, m_Data13, m_Data14, m_Data15);
}

} /* namespace event */ } /* namespace communication */ } /* namespace tsd */

#endif /* TSDTEMPLATEEVENT15_HPP_ */
