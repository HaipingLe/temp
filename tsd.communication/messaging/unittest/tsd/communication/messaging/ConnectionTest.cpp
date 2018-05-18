//////////////////////////////////////////////////////////////////////
/// @file ConnectionTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test Connection
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "ConnectionTest.hpp"
#include <gmock/gmock.h>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/ConnectionHelper.hpp>
#include <tsd/communication/messaging/TcpClientPort.hpp>
#include <tsd/communication/messaging/TcpServerPort.hpp>

namespace tsd {
namespace communication {
namespace messaging {

void ConnectionTest::setUp()
{
   m_TestObj.reset(new ConnectionHelper());
}

void ConnectionTest::tearDown()
{
   m_TestObj.reset();
   Router::cleanup();
   tsd::common::logging::LoggingManager::cleanup();
}

void ConnectionTest::test_SetConnected_JustRun_NothingHappens()
{
   // test execution
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->setConnected());
}

void ConnectionTest::test_SetDisconnected_JustRun_NothingHappens()
{
   // test execution
   // It is necessary to connect first. Otherwise assertion in IPort::disconnected() will not be met.
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->setConnected());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->setDisconnected());
}

void ConnectionTest::test_ProcessData_JustRun_NothingHappens()
{
   // function under test parameter preparation:
   const void* data{nullptr};
   size_t      size{};

   // test execution
   // It is necessary to connect first. Otherwise assertion in IPort::processData() will not be met.
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->setConnected());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->processData(data, size));
}

void ConnectionTest::test_GetData_JustRun_MemberPGetDataReturned()
{
   // function under test parameter preparation:
   const void* data{nullptr};
   size_t      size{};
   const bool  expectedResult{false};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, m_TestObj->getData(data, size));
}

void ConnectionTest::test_ConsumeData_JustRun_NothingHappens()
{
   // function under test parameter preparation:
   size_t amount{};

   // test execution
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->consumeData(amount));
}

void ConnectionTest::test_ConnectUpstream_JustRun_MemberPConnectUpstreamReturned()
{
   // function under test parameter preparation:
   const std::string subDomain{};
   const bool        expectedResult{false};

   EXPECT_CALL(*m_TestObj, setupConnection()).WillOnce(testing::DoDefault());

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, m_TestObj->connectUpstream(subDomain));
}

void ConnectionTest::test_ListenDownstream_JustRun_MemberPListenDownstreamReturned()
{
   const bool expectedResult{false};

   EXPECT_CALL(*m_TestObj, setupConnection()).WillOnce(testing::DoDefault());

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, m_TestObj->listenDownstream());
}

void ConnectionTest::test_Disconnect_JustRun_NothingHappens()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->disconnect());
}

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionTest);

} // namespace messaging
} // namespace communication
} // namespace tsd
