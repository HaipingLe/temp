//////////////////////////////////////////////////////////////////////
/// @file TsdEventTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test TsdEvent
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "TsdEventTest.hpp"
#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd {
namespace communication {
namespace event {

void TsdEventTest::setUp()
{
   constexpr uint32_t eventId = 0U;

   m_TestObj.reset(new TsdEvent(eventId));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id is not set", eventId, m_TestObj->getEventId());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender is not set to loopback address", LOOPBACK_ADDRESS, m_TestObj->getSenderAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver is not set to loopback address", LOOPBACK_ADDRESS, m_TestObj->getReceiverAddr());
}

void TsdEventTest::tearDown()
{
   m_TestObj.reset();
}

void TsdEventTest::test_Serialize_JustRun_NothingHappens()
{
   tsd::common::ipc::RpcBuffer rpcBuffer;

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Serialize is empty method", m_TestObj->serialize(rpcBuffer));
}

void TsdEventTest::test_Deserialize_JustRun_NothingHappens()
{
   tsd::common::ipc::RpcBuffer rpcBuffer;

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Deserialize is empty method", m_TestObj->deserialize(rpcBuffer));
}

void TsdEventTest::test_Clone_JustRun_ClonedObjectReturned()
{
   constexpr IfcAddr_t       sender(1U);
   constexpr IfcAddr_t       receiver(2U);
   constexpr uint32_t        eventId = 1U;
   TsdEvent                  testObj(eventId);
   std::unique_ptr<TsdEvent> cloneObject;

   testObj.setSenderAddr(sender);
   testObj.setReceiverAddr(receiver);
   cloneObject.reset(testObj.clone());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id is not copied", testObj.getEventId(), cloneObject->getEventId());
   // issue reported SUPMOB-1583, those fields are not copied
   //   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not copied", testObj.getSenderAddr(), cloneObject->getSenderAddr());
   //   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not copied", testObj.getReceiverAddr(), cloneObject->getReceiverAddr());
}

void TsdEventTest::test_GetEventId_JustRun_EventIdReturned()
{
   constexpr uint32_t expectedResult = 1U;
   TsdEvent           testObj(expectedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect event id returned", expectedResult, testObj.getEventId());
}

void TsdEventTest::test_SetSenderAddr_SetNewAddress_AddressSet()
{
   const IfcAddr_t expectedResult(1U);

   m_TestObj->setSenderAddr(expectedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not set", expectedResult, m_TestObj->getSenderAddr());
}

void TsdEventTest::test_SetReceiverAddr_SetNewAddress_AddressSet()
{
   const IfcAddr_t expectedResult(1U);

   m_TestObj->setReceiverAddr(expectedResult);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not set", expectedResult, m_TestObj->getReceiverAddr());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TsdEventTest);
void GlobalTsdEventTest::test_BitishiftLeftRightOperator_UseLeftAndRightShiftOperator_EventSerializedAndDeserialized()
{
   constexpr uint32_t          eventId = 1U;
   constexpr IfcAddr_t         sender(2U);
   constexpr IfcAddr_t         receiver(3U);
   constexpr size_t            size = 100;
   char                        buffer[size]{0};
   tsd::common::ipc::RpcBuffer rpcBuffer;
   TsdEvent                    eventToSerialize(eventId);
   TsdEvent                    eventToDeserialize(0U);

   rpcBuffer.init(buffer, size);
   eventToSerialize.setSenderAddr(sender);
   eventToSerialize.setReceiverAddr(receiver);

   rpcBuffer << eventToSerialize;
   rpcBuffer >> eventToDeserialize;
}

CPPUNIT_TEST_SUITE_REGISTRATION(GlobalTsdEventTest);
} // namespace event
} // namespace communication
} // namespace tsd

