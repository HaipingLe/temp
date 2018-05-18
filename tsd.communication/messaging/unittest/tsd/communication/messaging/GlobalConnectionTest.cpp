//////////////////////////////////////////////////////////////////////
/// @file GlobalConnectionTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test GlobalConnection
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "GlobalConnectionTest.hpp"
#include <gmock/gmock.h>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/ConnectionHelper.hpp>
#include <tsd/communication/messaging/TcpClientPort.hpp>
#include <tsd/communication/messaging/TcpServerPort.hpp>

namespace tsd {
namespace communication {
namespace messaging {

void GlobalConnectionTest::tearDown()
{
   Router::cleanup();
   tsd::common::logging::LoggingManager::cleanup();
}

void GlobalConnectionTest::test_ConnectUpstream_EmptyUrl_TcpConnectionEstablished()
{
   // function under test parameter preparation:
   const std::string url{};

// test execution
#ifdef TARGET_OS_POSIX_LINUX
   Router        r("r");
   TcpServerPort server(r);
#ifdef TARGET_TYPE_EMBEDDED
   server.initUnix("@");
#else  // !TARGET_TYPE_EMBEDDED
   server.initV4();
#endif // TARGET_TYPE_EMBEDDED
   std::shared_ptr<IConnection> connection(connectUpstream(url, server.getBoundPath()));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", connectUpstream(url, subDomain), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ConnectUpstream_UrlWithInvalidTcpAddress_ConnectionExceptionThrown()
{
   // function under test parameter preparation:
   const std::string url{"tcp://someName"};
   const std::string subDomain{};

   // test execution
   try
   {
      std::shared_ptr<IConnection> connection(connectUpstream(url, subDomain));
   }
   catch (ConnectionException e)
   {
#ifdef TARGET_OS_POSIX_LINUX
      std::string expectedExceptionMessage = "Could not parse: " + url;
#elif  // !TARGET_TYPE_EMBEDDED
      std::string expectedExceptionMessage = "Invalid address: " + url;
#endif // TARGET_OS_POSIX_LINUX
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values expected", expectedExceptionMessage, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void GlobalConnectionTest::test_ConnectUpstream_UrlWithValidTcpAddress_ConnectionEstablished()
{
   // function under test parameter preparation:
   std::string       url{"tcp://127.0.0.1:"};
   const std::string subDomain{};
   Router            r("r");
   TcpServerPort     server(r);
   server.initV4();
   std::stringstream ss;
   ss << server.getBoundPort();
   url += ss.str();

#ifdef TARGET_OS_POSIX_LINUX
   std::shared_ptr<IConnection> connection(connectUpstream(url, subDomain));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", connectUpstream(url, subDomain), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ConnectUpstream_EmptyUnixUrl_ConnectionEstablished()
{
   // function under test parameter preparation:
   std::string       url{"unix://"};
   const std::string subDomain{};
   Router            r("r");
   TcpServerPort     server(r);
   server.initUnix("@tsd.communication.commgr");

#ifdef TARGET_OS_POSIX_LINUX
   std::shared_ptr<IConnection> connection(connectUpstream(url, subDomain));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", connectUpstream(url, subDomain), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ConnectUpstream_SpecificUnixUrl_ConnectionEstablished()
{
   // function under test parameter preparation:
   std::string   path{"@test"};
   Router        r("r");
   TcpServerPort server(r);
   server.initUnix(path);
   std::string url{"unix://"};
   url += server.getBoundPath();
   const std::string subDomain{};

#ifdef TARGET_OS_POSIX_LINUX
   std::shared_ptr<IConnection> connection(connectUpstream(url, subDomain));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", connectUpstream(url, subDomain), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ConnectUpstream_UioUrl_ConnectionEstablished()
{
   /** Untestable due to SUPMOB-1623 */
   //   // function under test parameter preparation:
   //   const std::string url{"uio://"};
   //   const std::string subDomain{};

   //   // test execution
   //#ifdef TARGET_OS_POSIX_LINUX
   //   std::shared_ptr<IConnection> connection(connectUpstream(url, subDomain));
   //   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
   //#elif  // !TARGET_TYPE_EMBEDDED
   //   CPPUNIT_ASSERT_THROW_MESSAGE(
   //      "It is expected that this call will be terminated by an exception", connectUpstream(url, subDomain), ConnectionException);
   //#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ConnectUpstream_InvaidUrl_ConnectionExceptionThrown()
{
   // function under test parameter preparation:
   const std::string url{"some url"};
   const std::string subDomain;

   // test execution
   try
   {
      std::shared_ptr<IConnection> connection(connectUpstream(url, subDomain));
   }
   catch (ConnectionException e)
   {
      std::string expectedExceptionMessage = "Invalid address: " + url;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values expected", expectedExceptionMessage, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void GlobalConnectionTest::test_ListenDownstream_InvalidTcpUrl_ConnectionExceptionThrown()
{
   // function under test parameter preparation:
   const std::string url{"tcp://someAddress"};

   // test execution
   try
   {
      std::shared_ptr<IConnection> connection(listenDownstream(url));
   }
   catch (ConnectionException e)
   {
#ifdef TARGET_OS_POSIX_LINUX
      std::string expectedExceptionMessage = "Could not parse: " + url;
#elif  // !TARGET_TYPE_EMBEDDED
      std::string expectedExceptionMessage = "Invalid address: " + url;
#endif // TARGET_OS_POSIX_LINUX
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values expected", expectedExceptionMessage, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void GlobalConnectionTest::test_ListenDownstream_ValidTcpUrl_ConnectionEstablished()
{
   // function under test parameter preparation:
   const std::string url{"tcp://"};

// test execution
#ifdef TARGET_OS_POSIX_LINUX
   std::shared_ptr<IConnection> connection(listenDownstream(url));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", listenDownstream(url), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ListenDownstream_EmptyUnixUrl_ConnectionEstablished()
{
   // function under test parameter preparation:
   const std::string url{"unix://"};

// test execution
#ifdef TARGET_OS_POSIX_LINUX
   std::shared_ptr<IConnection> connection(listenDownstream(url));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", listenDownstream(url), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ListenDownstream_NonEmptyUnixUrl_ConnectionEstablished()
{
   // function under test parameter preparation:
   const std::string url{"unix://@test"};

// test execution
#ifdef TARGET_OS_POSIX_LINUX
   std::shared_ptr<IConnection> connection(listenDownstream(url));
   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
#elif  // !TARGET_TYPE_EMBEDDED
   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", listenDownstream(url), ConnectionException);
#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ListenDownstream_UioUrl_ConnectionEstablished()
{
   /** Untestable due to SUPMOB-1623 */
   //   // function under test parameter preparation:
   //   const std::string url{"uio://"};

   //   // test execution
   //#ifdef TARGET_OS_POSIX_LINUX
   //   std::shared_ptr<IConnection> connection(listenDownstream(url));
   //   CPPUNIT_ASSERT_MESSAGE("Not null expected", nullptr != connection);
   //#elif  // !TARGET_TYPE_EMBEDDED
   //   CPPUNIT_ASSERT_THROW_MESSAGE(
   //      "It is expected that this call will be terminated by an exception", listenDownstream(url), ConnectionException);
   //#endif // TARGET_OS_POSIX_LINUX
}

void GlobalConnectionTest::test_ListenDownstream_InvalidUrl_ConnectionExceptionThrown()
{
   // function under test parameter preparation:
   const std::string url{"Some Address"};

   // test execution
   try
   {
      std::shared_ptr<IConnection> connection(listenDownstream(url));
   }
   catch (ConnectionException e)
   {
      std::string expectedExceptionMessage = "Invalid address: " + url;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values expected", expectedExceptionMessage, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

CPPUNIT_TEST_SUITE_REGISTRATION(GlobalConnectionTest);

} // namespace messaging
} // namespace communication
} // namespace tsd
