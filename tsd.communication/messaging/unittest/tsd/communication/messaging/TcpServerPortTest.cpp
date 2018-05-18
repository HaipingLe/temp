//////////////////////////////////////////////////////////////////////
/// @file TcpServerPortTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test TcpServerPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "TcpServerPortTest.hpp"
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/TcpServerPort.hpp>

namespace tsd {
namespace communication {
namespace messaging {

TcpServerPortTest::~TcpServerPortTest()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void TcpServerPortTest::setUp()
{
   std::string testRouterName("testRouterName");
   m_TestRouter = std::make_shared<Router>(testRouterName);
   m_TestObject = std::make_shared<TcpServerPort>(*m_TestRouter.get());
}

void TcpServerPortTest::test_Constructor_CreateProvidedRouter_ExpectingObjectCreated()
{
   std::string                    testRouterName("testRouterName");
   Router                         testRouter(testRouterName);
   std::unique_ptr<TcpServerPort> testObject;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("TcpServerPort ctor failed with a throw", testObject.reset(new TcpServerPort(testRouter)));
   CPPUNIT_ASSERT_MESSAGE("Creation of TcpServerPort failed, and ptr is null", testObject.get() != nullptr);
}

void TcpServerPortTest::test_Disconnect_JustInvoke_ExpectingNoThrows()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("disconnect failed with a throw", m_TestObject->disconnect());
}

void TcpServerPortTest::test_InitV4_JustInvoke_ExpectingNoThrows()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initV4 failed with a throw", m_TestObject->initV4());
}

void TcpServerPortTest::test_InitUnix_JustInvoke_ExpectingNoThrows()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initUnix failed with a throw", m_TestObject->initUnix(""));
}

void TcpServerPortTest::test_GetBoundPort_JustInvoke_ExpectingDefaultValue()
{
   uint16_t expRet(0);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("getBoundPort returned unexpected value", expRet, m_TestObject->getBoundPort());
}

void TcpServerPortTest::test_GetBoundPath_JustInvoke_ExpectingEmptyStringReturned()
{
   std::string expRet("");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("getBoundPath returned unexpected value", expRet, m_TestObject->getBoundPath());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TcpServerPortTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
