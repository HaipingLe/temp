//////////////////////////////////////////////////////////////////////
/// @file TsdTemplateEvent9Test.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test TsdTemplateEvent9
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "TsdTemplateEvent9Test.hpp"
#include <tsd/communication/event/TsdTemplateEvent9.hpp>

namespace tsd {
namespace communication {
namespace event {

typedef TsdTemplateEvent9<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t> Uint32TsdTemplateEvent9;

namespace {
constexpr uint32_t EVENT_ID = 0U;
}

void TsdTemplateEvent9Test::test_Constructor_WithEventid_ObjectCreated()
{
   constexpr uint32_t      expectedValue{}; // initialized by default
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 1 is not initialized with default constructor", expectedValue, testObj.getData1());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 2 is not initialized with default constructor", expectedValue, testObj.getData2());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 3 is not initialized with default constructor", expectedValue, testObj.getData3());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 4 is not initialized with default constructor", expectedValue, testObj.getData4());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 5 is not initialized with default constructor", expectedValue, testObj.getData5());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 6 is not initialized with default constructor", expectedValue, testObj.getData6());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 7 is not initialized with default constructor", expectedValue, testObj.getData7());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 8 is not initialized with default constructor", expectedValue, testObj.getData8());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 9 is not initialized with default constructor", expectedValue, testObj.getData9());
}

void TsdTemplateEvent9Test::test_Constructor_WithEventidAndValues_ObjectCreated()
{
   const uint32_t          expectedValues[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
   Uint32TsdTemplateEvent9 testObj(EVENT_ID,
                                   expectedValues[0],
                                   expectedValues[1],
                                   expectedValues[2],
                                   expectedValues[3],
                                   expectedValues[4],
                                   expectedValues[5],
                                   expectedValues[6],
                                   expectedValues[7],
                                   expectedValues[8]);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 1 is initialized with incorrect value", expectedValues[0], testObj.getData1());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 2 is initialized with incorrect value", expectedValues[1], testObj.getData2());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 3 is initialized with incorrect value", expectedValues[2], testObj.getData3());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 4 is initialized with incorrect value", expectedValues[3], testObj.getData4());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 5 is initialized with incorrect value", expectedValues[4], testObj.getData5());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 6 is initialized with incorrect value", expectedValues[5], testObj.getData6());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 7 is initialized with incorrect value", expectedValues[6], testObj.getData7());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 8 is initialized with incorrect value", expectedValues[7], testObj.getData8());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 9 is initialized with incorrect value", expectedValues[8], testObj.getData9());
}

void TsdTemplateEvent9Test::test_Serialize_SerializeAndDeserializeData_DataDeserialized()
{
   constexpr size_t            size = 100;
   char                        buffer[size]{0};
   const uint32_t              expectedValues[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
   tsd::common::ipc::RpcBuffer rpcBuffer;
   Uint32TsdTemplateEvent9     actualResult(EVENT_ID);
   Uint32TsdTemplateEvent9     testObj(EVENT_ID,
                                   expectedValues[0],
                                   expectedValues[1],
                                   expectedValues[2],
                                   expectedValues[3],
                                   expectedValues[4],
                                   expectedValues[5],
                                   expectedValues[6],
                                   expectedValues[7],
                                   expectedValues[8]);
   rpcBuffer.init(buffer, size);

   testObj.serialize(rpcBuffer);
   rpcBuffer.setPos(0);
   actualResult.deserialize(rpcBuffer);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 1", actualResult.getData1(), testObj.getData1());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 2", actualResult.getData2(), testObj.getData2());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 3", actualResult.getData3(), testObj.getData3());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 4", actualResult.getData4(), testObj.getData4());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 5", actualResult.getData5(), testObj.getData5());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 6", actualResult.getData6(), testObj.getData6());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 7", actualResult.getData7(), testObj.getData7());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 8", actualResult.getData8(), testObj.getData8());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 9", actualResult.getData9(), testObj.getData9());
}

void TsdTemplateEvent9Test::test_Clone_JustRun_ColonedObjectReturned()
{
   const uint32_t                           expectedValues[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
   std::unique_ptr<Uint32TsdTemplateEvent9> cloneObj;
   Uint32TsdTemplateEvent9                  testObj(EVENT_ID,
                                   expectedValues[0],
                                   expectedValues[1],
                                   expectedValues[2],
                                   expectedValues[3],
                                   expectedValues[4],
                                   expectedValues[5],
                                   expectedValues[6],
                                   expectedValues[7],
                                   expectedValues[8]);

   cloneObj.reset(dynamic_cast<Uint32TsdTemplateEvent9*>(testObj.clone()));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 1", cloneObj->getData1(), testObj.getData1());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 2", cloneObj->getData2(), testObj.getData2());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 3", cloneObj->getData3(), testObj.getData3());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 4", cloneObj->getData4(), testObj.getData4());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 5", cloneObj->getData5(), testObj.getData5());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 6", cloneObj->getData6(), testObj.getData6());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 7", cloneObj->getData7(), testObj.getData7());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 8", cloneObj->getData8(), testObj.getData8());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 9", cloneObj->getData9(), testObj.getData9());
}

void TsdTemplateEvent9Test::test_SetData1_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData1(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 1 was not set", expecetedResult, testObj.getData1());
}

void TsdTemplateEvent9Test::test_SetData2_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData2(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 2 was not set", expecetedResult, testObj.getData2());
}

void TsdTemplateEvent9Test::test_SetData3_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData3(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 3 was not set", expecetedResult, testObj.getData3());
}

void TsdTemplateEvent9Test::test_SetData4_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData4(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 4 was not set", expecetedResult, testObj.getData4());
}

void TsdTemplateEvent9Test::test_SetData5_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData5(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 5 was not set", expecetedResult, testObj.getData5());
}

void TsdTemplateEvent9Test::test_SetData6_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData6(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 6 was not set", expecetedResult, testObj.getData6());
}

void TsdTemplateEvent9Test::test_SetData7_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData7(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 7 was not set", expecetedResult, testObj.getData7());
}

void TsdTemplateEvent9Test::test_SetData8_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData8(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 8 was not set", expecetedResult, testObj.getData8());
}

void TsdTemplateEvent9Test::test_SetData9_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent9 testObj(EVENT_ID);

   testObj.setData9(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 9 was not set", expecetedResult, testObj.getData9());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TsdTemplateEvent9Test);
} // namespace event
} // namespace communication
} // namespace tsd
