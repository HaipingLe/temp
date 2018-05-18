//////////////////////////////////////////////////////////////////////
/// @file TsdEventTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test TsdEvent
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_EVENT_TSDEVENTTEST_HPP
#define TSD_COMMUNICATION_EVENT_TSDEVENTTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace event {

class TsdEvent;

/**
 * Testclass for TsdEvent
 *
 * @brief Testclass for TsdEvent
 */
class TsdEventTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::Serialize
    * @tsd_testexpected nothing happens
    */
   void test_Serialize_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::Deserialize
    * @tsd_testexpected nothing happens
    */
   void test_Deserialize_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::Clone
    * @tsd_testexpected cloned object returned
    */
   void test_Clone_JustRun_ClonedObjectReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::GetEventId
    * @tsd_testexpected event id returned
    */
   void test_GetEventId_JustRun_EventIdReturned();
   /**
    * @brief Test scenario: set new address
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::SetSenderAddr
    * @tsd_testexpected address set
    */
   void test_SetSenderAddr_SetNewAddress_AddressSet();
   /**
    * @brief Test scenario: set new address
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::SetReceiverAddr
    * @tsd_testexpected address set
    */
   void test_SetReceiverAddr_SetNewAddress_AddressSet();

private:
   std::unique_ptr<TsdEvent> m_TestObj;

   CPPUNIT_TEST_SUITE(TsdEventTest);
   CPPUNIT_TEST(test_Serialize_JustRun_NothingHappens);
   CPPUNIT_TEST(test_Deserialize_JustRun_NothingHappens);
   CPPUNIT_TEST(test_Clone_JustRun_ClonedObjectReturned);
   CPPUNIT_TEST(test_GetEventId_JustRun_EventIdReturned);
   CPPUNIT_TEST(test_SetSenderAddr_SetNewAddress_AddressSet);
   CPPUNIT_TEST(test_SetReceiverAddr_SetNewAddress_AddressSet);
   CPPUNIT_TEST_SUITE_END();
};

/**
 * Testclass for GlobalTsdEvent
 *
 * @brief Testclass for GlobalTsdEvent
 */
class GlobalTsdEventTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: use left and right shift operator
    *
    * @tsd_testobject tsd::communication::event::TsdEvent::BitishiftLeftRightOperator
    * @tsd_testexpected event serialized and deserialized
    */
   void test_BitishiftLeftRightOperator_UseLeftAndRightShiftOperator_EventSerializedAndDeserialized();

   CPPUNIT_TEST_SUITE(GlobalTsdEventTest);
   CPPUNIT_TEST(test_BitishiftLeftRightOperator_UseLeftAndRightShiftOperator_EventSerializedAndDeserialized);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace event
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_EVENT_TSDEVENTTEST_HPP
