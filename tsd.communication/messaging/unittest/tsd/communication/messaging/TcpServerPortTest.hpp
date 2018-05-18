//////////////////////////////////////////////////////////////////////
/// @file TcpServerPortTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test TcpServerPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_TCPSERVERPORTTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPSERVERPORTTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

class TcpServerPort;
class Router;

/**
 * Testclass for TcpServerPort
 *
 * @brief Testclass for TcpServerPort
 */
class TcpServerPortTest : public CPPUNIT_NS::TestFixture
{
public:

   /**
    * @brief Destructor to clear singletons.
    */
   virtual ~TcpServerPortTest();

   /**
    * @brief setUp function called before every test
    */
   void setUp() override;

   /**
    * @brief Test scenario: create provided router
    *
    * @tsd_testobject tsd::communication::messaging::TcpServerPort::Constructor
    * @tsd_testexpected expecting object created
    */
   void test_Constructor_CreateProvidedRouter_ExpectingObjectCreated();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::TcpServerPort::Disconnect
    * @tsd_testexpected expecting no throws
    */
   void test_Disconnect_JustInvoke_ExpectingNoThrows();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::TcpServerPort::InitV4
    * @tsd_testexpected expecting no throws
    */
   void test_InitV4_JustInvoke_ExpectingNoThrows();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::TcpServerPort::InitUnix
    * @tsd_testexpected expecting no throws
    */
   void test_InitUnix_JustInvoke_ExpectingNoThrows();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::TcpServerPort::GetBoundPort
    * @tsd_testexpected expecting default value
    */
   void test_GetBoundPort_JustInvoke_ExpectingDefaultValue();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::TcpServerPort::GetBoundPath
    * @tsd_testexpected expecting empty string returned
    */
   void test_GetBoundPath_JustInvoke_ExpectingEmptyStringReturned();

   std::shared_ptr<TcpServerPort> m_TestObject;
   std::shared_ptr<Router>        m_TestRouter;

   CPPUNIT_TEST_SUITE(TcpServerPortTest);
   CPPUNIT_TEST(test_Constructor_CreateProvidedRouter_ExpectingObjectCreated);
   CPPUNIT_TEST(test_Disconnect_JustInvoke_ExpectingNoThrows);
   CPPUNIT_TEST(test_InitV4_JustInvoke_ExpectingNoThrows);
   CPPUNIT_TEST(test_InitUnix_JustInvoke_ExpectingNoThrows);
   CPPUNIT_TEST(test_GetBoundPort_JustInvoke_ExpectingDefaultValue);
   CPPUNIT_TEST(test_GetBoundPath_JustInvoke_ExpectingEmptyStringReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_TCPSERVERPORTTEST_HPP
