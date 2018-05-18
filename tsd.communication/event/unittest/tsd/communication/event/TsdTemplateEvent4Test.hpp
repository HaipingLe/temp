//////////////////////////////////////////////////////////////////////
/// @file TsdTemplateEvent4Test.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test TsdTemplateEvent4
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_EVENT_TSDTEMPLATEEVENT4TEST_HPP
#define TSD_COMMUNICATION_EVENT_TSDTEMPLATEEVENT4TEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace event {

/**
 * Testclass for TsdTemplateEvent4
 *
 * @brief Testclass for TsdTemplateEvent4
 */
class TsdTemplateEvent4Test : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with eventid
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithEventid_ObjectCreated();
   /**
    * @brief Test scenario: with eventid and values
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithEventidAndValues_ObjectCreated();
   /**
    * @brief Test scenario: serialize and deserialize data
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::Serialize
    * @tsd_testexpected data deserialized
    */
   void test_Serialize_SerializeAndDeserializeData_DataDeserialized();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::Clone
    * @tsd_testexpected cloned object returned
    */
   void test_Clone_JustRun_ClonedObjectReturned();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::SetData1
    * @tsd_testexpected new value set
    */
   void test_SetData1_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::SetData2
    * @tsd_testexpected new value set
    */
   void test_SetData2_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::SetData3
    * @tsd_testexpected new value set
    */
   void test_SetData3_SetNewValue_NewValueSet();
   /**
    * @brief Test scenario: set new value
    *
    * @tsd_testobject tsd::communication::event::TsdTemplateEvent4::SetData4
    * @tsd_testexpected new value set
    */
   void test_SetData4_SetNewValue_NewValueSet();

   CPPUNIT_TEST_SUITE(TsdTemplateEvent4Test);
   CPPUNIT_TEST(test_Constructor_WithEventid_ObjectCreated);
   CPPUNIT_TEST(test_Constructor_WithEventidAndValues_ObjectCreated);
   CPPUNIT_TEST(test_Serialize_SerializeAndDeserializeData_DataDeserialized);
   CPPUNIT_TEST(test_Clone_JustRun_ClonedObjectReturned);
   CPPUNIT_TEST(test_SetData1_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData2_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData3_SetNewValue_NewValueSet);
   CPPUNIT_TEST(test_SetData4_SetNewValue_NewValueSet);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace event
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_EVENT_TSDTEMPLATEEVENT4TEST_HPP
