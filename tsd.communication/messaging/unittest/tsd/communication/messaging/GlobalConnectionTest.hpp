//////////////////////////////////////////////////////////////////////
/// @file GlobalConnectionTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test GlobalConnection
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_GLOBALCONNECTIONTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_GLOBALCONNECTIONTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for GlobalConnection
 *
 * @brief Testclass for GlobalConnection
 */
class GlobalConnectionTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;
   /**
    * @brief Test scenario: empty url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected tcp connection established
    */
   void test_ConnectUpstream_EmptyUrl_TcpConnectionEstablished();
   /**
    * @brief Test scenario: url with invalid tcp address
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ConnectUpstream_UrlWithInvalidTcpAddress_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: url with valid tcp address
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected connection established
    */
   void test_ConnectUpstream_UrlWithValidTcpAddress_ConnectionEstablished();
   /**
    * @brief Test scenario: empty unix url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected connection established
    */
   void test_ConnectUpstream_EmptyUnixUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: specific unix url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected connection established
    */
   void test_ConnectUpstream_SpecificUnixUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: uio url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected connection established
    */
   void test_ConnectUpstream_UioUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: invaid url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ConnectUpstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ConnectUpstream_InvaidUrl_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: invalid tcp url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ListenDownstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ListenDownstream_InvalidTcpUrl_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: valid tcp url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ListenDownstream
    * @tsd_testexpected connection established
    */
   void test_ListenDownstream_ValidTcpUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: empty unix url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ListenDownstream
    * @tsd_testexpected connection established
    */
   void test_ListenDownstream_EmptyUnixUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: non empty unix url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ListenDownstream
    * @tsd_testexpected connection established
    */
   void test_ListenDownstream_NonEmptyUnixUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: uio url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ListenDownstream
    * @tsd_testexpected connection established
    */
   void test_ListenDownstream_UioUrl_ConnectionEstablished();
   /**
    * @brief Test scenario: invalid url
    *
    * @tsd_testobject tsd::communication::messaging::GlobalConnection::ListenDownstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ListenDownstream_InvalidUrl_ConnectionExceptionThrown();

   CPPUNIT_TEST_SUITE(GlobalConnectionTest);
   CPPUNIT_TEST(test_ConnectUpstream_EmptyUrl_TcpConnectionEstablished);
   CPPUNIT_TEST(test_ConnectUpstream_UrlWithInvalidTcpAddress_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ConnectUpstream_UrlWithValidTcpAddress_ConnectionEstablished);
   CPPUNIT_TEST(test_ConnectUpstream_EmptyUnixUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ConnectUpstream_SpecificUnixUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ConnectUpstream_UioUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ConnectUpstream_InvaidUrl_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ListenDownstream_InvalidTcpUrl_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ListenDownstream_ValidTcpUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ListenDownstream_EmptyUnixUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ListenDownstream_NonEmptyUnixUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ListenDownstream_UioUrl_ConnectionEstablished);
   CPPUNIT_TEST(test_ListenDownstream_InvalidUrl_ConnectionExceptionThrown);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_GLOBALCONNECTIONTEST_HPP
