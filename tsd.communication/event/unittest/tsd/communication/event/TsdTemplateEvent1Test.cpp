//////////////////////////////////////////////////////////////////////
/// @file TsdTemplateEvent1Test.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test TsdTemplateEvent1
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "TsdTemplateEvent1Test.hpp"
#include <tsd/communication/event/TsdTemplateEvent1.hpp>

namespace tsd {
namespace communication {
namespace event {

typedef TsdTemplateEvent1<uint32_t> Uint32TsdTemplateEvent1;

namespace {
constexpr uint32_t EVENT_ID = 0U;
}

void TsdTemplateEvent1Test::test_Constructor_WithEventid_ObjectCreated()
{
   constexpr uint32_t      expectedValue{}; // initialized by default
   Uint32TsdTemplateEvent1 testObj(EVENT_ID);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 1 is not initialized with default constructor", expectedValue, testObj.getData1());
}

void TsdTemplateEvent1Test::test_Constructor_WithEventidAndValue_ObjectCreated()
{
   constexpr uint32_t      expectedValue = 1U;
   Uint32TsdTemplateEvent1 testObj(EVENT_ID, expectedValue);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 1 is initialized with incorrect value", expectedValue, testObj.getData1());
}

void TsdTemplateEvent1Test::test_Serialize_SerializeAndDeserializeData_DataDeserialized()
{
   constexpr size_t            size = 100;
   char                        buffer[size]{0};
   constexpr uint32_t          expectedValue = 1U;
   tsd::common::ipc::RpcBuffer rpcBuffer;
   Uint32TsdTemplateEvent1     actualResult(EVENT_ID);
   Uint32TsdTemplateEvent1     testObj(EVENT_ID, expectedValue);
   rpcBuffer.init(buffer, size);

   testObj.serialize(rpcBuffer);
   rpcBuffer.setPos(0);
   actualResult.deserialize(rpcBuffer);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to serialize/deserialize data 1", actualResult.getData1(), testObj.getData1());
}

void TsdTemplateEvent1Test::test_Clone_JustRun_ClonedObjectReturned()
{
   constexpr uint32_t                       expectedValue = 1U;
   std::unique_ptr<Uint32TsdTemplateEvent1> cloneObj;
   Uint32TsdTemplateEvent1                  testObj(EVENT_ID, expectedValue);

   cloneObj.reset(dynamic_cast<Uint32TsdTemplateEvent1*>(testObj.clone()));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Failed to clone data 1", cloneObj->getData1(), testObj.getData1());
}

void TsdTemplateEvent1Test::test_SetData1_SetNewValue_NewValueSet()
{
   constexpr uint32_t      expecetedResult = 1U;
   Uint32TsdTemplateEvent1 testObj(EVENT_ID);

   testObj.setData1(expecetedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Data 1 was not set", expecetedResult, testObj.getData1());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TsdTemplateEvent1Test);
} // namespace event
} // namespace communication
} // namespace tsd
