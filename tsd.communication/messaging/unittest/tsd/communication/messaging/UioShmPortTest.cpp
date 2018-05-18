//////////////////////////////////////////////////////////////////////
/// @file UioShmPortTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test UioShmPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "UioShmPortTest.hpp"
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/Router.hpp>
#include <tsd/communication/messaging/UioShmPort.hpp>
#include <unistd.h>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
const std::string DEFAULT_SUBDOMAIN_NAME = "TestSubDomain";
}

UioShmPortTest::~UioShmPortTest()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void UioShmPortTest::setUp()
{
   Router router("UioShmPortTest");
   m_TestObj = std::make_shared<UioShmPort>(router);
   strncpy(m_fName, "/tmp/fileXXXXXX", sizeof(m_fName));
   mkstemp(m_fName);
}

void UioShmPortTest::tearDown()
{
   m_TestObj.reset();
   unlink(m_fName);
}

void UioShmPortTest::test_Constructor_WithRouter_ObjectCreated()
{
   Router router("UioShmPortTest");
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", m_TestObj = std::make_shared<UioShmPort>(router));
}

void UioShmPortTest::test_ConnectUpstream_MemberConnector_ConnectionExceptionThrown()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //      CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //      m_TestObj->connectUpstream(m_fName, DEFAULT_SUBDOMAIN_NAME));
   //      CPPUNIT_ASSERT_THROW_MESSAGE("Connector member should already exist", m_TestObj->connectUpstream(m_fName,
   //      DEFAULT_SUBDOMAIN_NAME), ConnectionException);
}

void UioShmPortTest::test_ConnectUpstream_NotMemberConnectorAndMemberListener_ConnectionExceptionThrown()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->listenDownstream(m_fName));
   //   CPPUNIT_ASSERT_THROW_MESSAGE("Listener member should already exist", m_TestObj->connectUpstream(m_fName,
   //   DEFAULT_SUBDOMAIN_NAME), ConnectionException);
}

void UioShmPortTest::test_ConnectUpstream_NotMemberConnectorAndNotMemberListener_NothingHappens()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->connectUpstream(m_fName, DEFAULT_SUBDOMAIN_NAME));
}

void UioShmPortTest::test_ListenDownstream_MemberListener_ConnectionExceptionThrown()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->listenDownstream(m_fName));
   //   CPPUNIT_ASSERT_THROW_MESSAGE("Listener member should already exist", m_TestObj->listenDownstream(m_fName),
   //   ConnectionException);
}

void UioShmPortTest::test_ListenDownstream_NotMemberListenerAndMemberConnector_ConnectionExceptionThrown()
{
   //   cannot be executed because of bugs:
   //   SUBMOP-1622
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->connectUpstream(m_fName, DEFAULT_SUBDOMAIN_NAME));
   //   CPPUNIT_ASSERT_THROW_MESSAGE("Connector member should already exist", m_TestObj->listenDownstream(m_fName),
   //   ConnectionException);
}

void UioShmPortTest::test_ListenDownstream_NotMemberListenerAndNotMemberConnector_NothingHappens()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->listenDownstream(m_fName));
}

void UioShmPortTest::test_Disconnect_WithoutListenerAndConnector_NothingHappens()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->disconnect());
}

void UioShmPortTest::test_Disconnect_WithListener_NothingHappens()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->listenDownstream(m_fName));
   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->disconnect());
}

void UioShmPortTest::test_Disconnect_WithConnector_NothingHappens()
{
   //   cannot be executed because of bug:
   //   SUBMOP-1623

   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
   //   m_TestObj->connectUpstream(m_fName, DEFAULT_SUBDOMAIN_NAME));
   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->disconnect());
}

CPPUNIT_TEST_SUITE_REGISTRATION(UioShmPortTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
