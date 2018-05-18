//////////////////////////////////////////////////////////////////////
/// @file QueueTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test Queue
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "QueueTest.hpp"
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/IQueueMock.hpp>
#include <tsd/communication/messaging/Queue.hpp>
#include <tsd/communication/messaging/Router.hpp>

namespace tsd {
namespace communication {
namespace messaging {

void GlobalQueueTest::test_CreateQueue_InvokeProvidedName_ExpectingValidPointerReturned()
{
   std::unique_ptr<IQueue> retQueue;
   std::string             testQueueName("TestQueue");
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Creating queue failed with a throw", retQueue.reset(createQueue(testQueueName)));
   CPPUNIT_ASSERT_MESSAGE("returned queue null unexpectedly", nullptr != retQueue);
}

void GlobalQueueTest::tearDown()
{
   Router::cleanup();
   tsd::common::logging::LoggingManager::cleanup();
}

CPPUNIT_TEST_SUITE_REGISTRATION(GlobalQueueTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
