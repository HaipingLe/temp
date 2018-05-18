//////////////////////////////////////////////////////////////////////
/// @file TcpClientPortTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test TcpClientPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_TCPCLIENTPORTTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPCLIENTPORTTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for TcpClientPort
 *
 * @brief Testclass for TcpClientPort
 */
class TcpClientPortTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;
   /**
    * @brief Test scenario: with router and sub domain
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithRouterAndSubDomain_ObjectCreated();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::Disconnect
    * @tsd_testexpected nothing happens
    */
   void test_Disconnect_JustRun_NothingHappens();
   /**
    * @brief Test scenario: init failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitV4
    * @tsd_testexpected connection exception thrown
    */
   void test_InitV4_InitFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: socket creation failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitV4
    * @tsd_testexpected connection exception thrown
    */
   void test_InitV4_SocketCreationFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: no connection
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitV4
    * @tsd_testexpected connection exception thrown
    */
   void test_InitV4_NoConnection_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: tcp endpoint init failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitV4
    * @tsd_testexpected connection exception thrown
    */
   void test_InitV4_TcpEndpointInitFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: upstream init failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitV4
    * @tsd_testexpected connection exception thrown
    */
   void test_InitV4_UpstreamInitFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: no errors
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitV4
    * @tsd_testexpected connection established
    */
   void test_InitV4_NoErrors_ConnectionEstablished();
   /**
    * @brief Test scenario: selector init failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitUnix
    * @tsd_testexpected connection exception thrown
    */
   void test_InitUnix_SelectorInitFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: socket creation failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitUnix
    * @tsd_testexpected connection exception thrown
    */
   void test_InitUnix_SocketCreationFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: connection failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitUnix
    * @tsd_testexpected connection exception thrown
    */
   void test_InitUnix_ConnectionFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: tcp endpoint failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitUnix
    * @tsd_testexpected connection exception thrown
    */
   void test_InitUnix_TcpEndpointFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: upstream inif failed
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitUnix
    * @tsd_testexpected connection exception thrown
    */
   void test_InitUnix_UpstreamInifFailed_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: no errors
    *
    * @tsd_testobject tsd::communication::messaging::TcpClientPort::InitUnix
    * @tsd_testexpected connection established
    */
   void test_InitUnix_NoErrors_ConnectionEstablished();

   CPPUNIT_TEST_SUITE(TcpClientPortTest);
   CPPUNIT_TEST(test_Constructor_WithRouterAndSubDomain_ObjectCreated);
   CPPUNIT_TEST(test_Disconnect_JustRun_NothingHappens);
   CPPUNIT_TEST(test_InitV4_InitFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitV4_SocketCreationFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitV4_NoConnection_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitV4_TcpEndpointInitFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitV4_UpstreamInitFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitV4_NoErrors_ConnectionEstablished);
   CPPUNIT_TEST(test_InitUnix_SelectorInitFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitUnix_SocketCreationFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitUnix_ConnectionFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitUnix_TcpEndpointFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitUnix_UpstreamInifFailed_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_InitUnix_NoErrors_ConnectionEstablished);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_TCPCLIENTPORTTEST_HPP
