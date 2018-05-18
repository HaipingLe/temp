//////////////////////////////////////////////////////////////////////
/// @file SocketHelperTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test SocketHelper
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "SocketHelperTest.hpp"
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/SocketHelper.hpp>

namespace tsd {
namespace communication {
namespace messaging {

void GlobalSocketHelperTest::test_ParseUnixPath_PathLengthIsGreaterThanSizeofMinus1_ConnectionExceptionThrown()
{
   std::string path = "testName";
   sockaddr_un sockaddr;
   socklen_t   sockaddrLen;

   path.resize(sizeof(sockaddr.sun_path), 't');
   CPPUNIT_ASSERT_MESSAGE("Path is shorter than sockAddr", path.length() > sizeof(sockaddr.sun_path) - 1);

   CPPUNIT_ASSERT_THROW_MESSAGE(
      "It is expected that this call will be terminated by an exception", parseUnixPath(path, sockaddr, sockaddrLen), ConnectionException);
}

void GlobalSocketHelperTest::test_ParseUnixPath_FirstCharOfSunPathIsNotAsExpected_FalseReturned()
{
   std::string path = "#testName";
   sockaddr_un sockaddr;
   socklen_t   sockaddrLen;

   CPPUNIT_ASSERT_EQUAL_MESSAGE("First character of UNIX Socket path is not as expected", '#', path[0]);
   CPPUNIT_ASSERT_MESSAGE("Path is longer than sockAddr", path.length() <= sizeof(sockaddr.sun_path) - 1);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", false, parseUnixPath(path, sockaddr, sockaddrLen));
   CPPUNIT_ASSERT_MESSAGE("UNIX Socket family is Af Unix", AF_UNIX == sockaddr.sun_family);

   socklen_t expectedSockaddrLen = static_cast<socklen_t>(sizeof(sa_family_t) + path.length() + 1u);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("SockaddrLen is not as expected", expectedSockaddrLen, sockaddrLen);
}

void GlobalSocketHelperTest::test_ParseUnixPath_PathLongerThanOneChar_TrueReturned()
{
   std::string path = "@testName";
   sockaddr_un sockaddr;
   socklen_t   sockaddrLen;

   CPPUNIT_ASSERT_EQUAL_MESSAGE("First character of UNIX Socket path is not as expected", '@', path[0]);
   CPPUNIT_ASSERT_MESSAGE("Path is longer than sockAddr", path.length() <= sizeof(sockaddr.sun_path) - 1);
   CPPUNIT_ASSERT_MESSAGE("Path is equal to 1", path.length() != 1);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", true, parseUnixPath(path, sockaddr, sockaddrLen));
   CPPUNIT_ASSERT_MESSAGE("UNIX Socket family is Af Unix", AF_UNIX == sockaddr.sun_family);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("First character of UNIX Socket path is not as expected", '\0', sockaddr.sun_path[0]);

   socklen_t expectedSockaddrLen = static_cast<socklen_t>(sizeof(sa_family_t) + path.length());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("SockaddrLen is not as expected", expectedSockaddrLen, sockaddrLen);
}

void GlobalSocketHelperTest::test_ParseUnixPath_PathLengthIsOne_TrueReturned()
{
   std::string path = "@";
   sockaddr_un sockaddr;
   socklen_t   sockaddrLen;

   CPPUNIT_ASSERT_EQUAL_MESSAGE("First character of UNIX Socket path is not as expected", '@', path[0]);
   CPPUNIT_ASSERT_MESSAGE("Path is longer than sockAddr", path.length() <= sizeof(sockaddr.sun_path) - 1);
   CPPUNIT_ASSERT_MESSAGE("Path is not equal to 1", path.length() == 1);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", true, parseUnixPath(path, sockaddr, sockaddrLen));
   CPPUNIT_ASSERT_MESSAGE("UNIX Socket family is Af Unix", AF_UNIX == sockaddr.sun_family);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("First character of UNIX Socket path is not as expected", '@', sockaddr.sun_path[0]);

   socklen_t expectedSockaddrLen = sizeof(sa_family_t);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("SockaddrLen is not as expected", expectedSockaddrLen, sockaddrLen);
}

CPPUNIT_TEST_SUITE_REGISTRATION(GlobalSocketHelperTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
