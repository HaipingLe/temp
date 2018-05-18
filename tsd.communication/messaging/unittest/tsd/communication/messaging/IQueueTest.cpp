//////////////////////////////////////////////////////////////////////
/// @file IQueueTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test IQueue
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "IQueueTest.hpp"
#include <tsd/communication/messaging/IQueue.hpp>
#include <tsd/communication/messaging/IQueueMock.hpp>

namespace tsd {
namespace communication {
namespace messaging {

void IMessageFactoryTest::test_GetEventIds_JustRun_StdVectorReturned()
{
   // jira-bug PRE06-5789
   IMessageFactoryMock testObj;
   EXPECT_CALL(testObj, getEventIds()).WillOnce(testing::InvokeWithoutArgs(&testObj, &IMessageFactoryMock::doGetEventIds));

   std::vector<uint32_t> result = testObj.getEventIds();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returned vector is not empty", size_t{0}, result.size());
   CPPUNIT_ASSERT_MESSAGE("Mock was not verified", testing::Mock::VerifyAndClearExpectations(&testObj));
}
CPPUNIT_TEST_SUITE_REGISTRATION(IMessageFactoryTest);

void IQueueTest::test_Destructor_JustRun_ObjectDestroyed()
{
   IQueue* testObj;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception thrown", testObj = new IQueueMock());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception thrown", delete testObj);
}

CPPUNIT_TEST_SUITE_REGISTRATION(IQueueTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
