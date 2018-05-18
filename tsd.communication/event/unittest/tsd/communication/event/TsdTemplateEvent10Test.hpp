//////////////////////////////////////////////////////////////////////
/// @file TsdTemplateEvent10Test.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test TsdTemplateEvent10
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_EVENT_TSDTEMPLATEEVENT10TEST_HPP
#define TSD_COMMUNICATION_EVENT_TSDTEMPLATEEVENT10TEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace event {

/**
 * Testclass for TsdTemplateEvent10
 *
 * @brief Testclass for TsdTemplateEvent10
 */
class TsdTemplateEvent10Test : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with eventid
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithEventid_ObjectCreated();
   /**
    * @brief Test scenario: with eventid and values
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithEventidAndValues_ObjectCreated();
   /**
    * @brief Test scenario: serialize and deserialize data
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::Serialize
    * @tsd_testexpected data deserialized
    */
   void test_Serialize_SerializeAndDeserializeData_DataDeserialized();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::Clone
    * @tsd_testexpected new returned
    */
   void test_Clone_JustRun_NewReturned();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData1
    * @tsd_testexpected new value set
    */
   void test_SetData1_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData2
    * @tsd_testexpected new value set
    */
   void test_SetData2_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData3
    * @tsd_testexpected new value set
    */
   void test_SetData3_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData4
    * @tsd_testexpected new value set
    */
   void test_SetData4_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData5
    * @tsd_testexpected new value set
    */
   void test_SetData5_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData6
    * @tsd_testexpected new value set
    */
   void test_SetData6_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData7
    * @tsd_testexpected new value set
    */
   void test_SetData7_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData8
    * @tsd_testexpected new value set
    */
   void test_SetData8_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData9
    * @tsd_testexpected new value set
    */
   void test_SetData9_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent10::SetData10
    * @tsd_testexpected new value set
    */
   void test_SetData10_SetNewValue_NewValueSet();

   CPPUNIT_TEST_SUITE(TsdTemplateEvent10Test);
   CPPUNIT_TEST(test_Constructor_WithEventid_ObjectCreated);
   CPPUNIT_TEST(test_Constructor_WithEventidAndValues_ObjectCreated);
   CPPUNIT_TEST(test_Serialize_SerializeAndDeserializeData_DataDeserialized);
   CPPUNIT_TEST(test_Clone_JustRun_NewReturned);
   CPPUNIT_TEST(test_SetData1_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData2_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData3_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData4_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData5_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData6_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData7_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData8_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData9_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData10_SetNewValue_NewValueSet);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace event
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_EVENT_TSDTEMPLATEEVENT10TEST_HPP
