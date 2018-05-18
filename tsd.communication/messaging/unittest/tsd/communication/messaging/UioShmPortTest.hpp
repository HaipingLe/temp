//////////////////////////////////////////////////////////////////////
/// @file UioShmPortTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test UioShmPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_UIOSHMPORTTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_UIOSHMPORTTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

class UioShmPort;
class Router;

/**
 * Testclass for UioShmPort
 *
 * @brief Testclass for UioShmPort
 */
class UioShmPortTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Destructor to clear singletons.
    */
   virtual ~UioShmPortTest();

   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;

   /**
    * @brief Test scenario: with router
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithRouter_ObjectCreated();
   /**
    * @brief Test scenario: member connector
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::ConnectUpstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ConnectUpstream_MemberConnector_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: not member connector and member listener
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::ConnectUpstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ConnectUpstream_NotMemberConnectorAndMemberListener_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: not member connector and not member listener
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::ConnectUpstream
    * @tsd_testexpected nothing happens
    */
   void test_ConnectUpstream_NotMemberConnectorAndNotMemberListener_NothingHappens();
   /**
    * @brief Test scenario: member listener
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::ListenDownstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ListenDownstream_MemberListener_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: not member listener and member connector
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::ListenDownstream
    * @tsd_testexpected connection exception thrown
    */
   void test_ListenDownstream_NotMemberListenerAndMemberConnector_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: not member listener and not member connector
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::ListenDownstream
    * @tsd_testexpected nothing happens
    */
   void test_ListenDownstream_NotMemberListenerAndNotMemberConnector_NothingHappens();
   /**
    * @brief Test scenario: without listener and connector
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::Disconnect
    * @tsd_testexpected nothing happens
    */
   void test_Disconnect_WithoutListenerAndConnector_NothingHappens();
   /**
    * @brief Test scenario: with listener
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::Disconnect
    * @tsd_testexpected nothing happens
    */
   void test_Disconnect_WithListener_NothingHappens();
   /**
    * @brief Test scenario: with connector
    *
    * @tsd_testobject tsd::communication::messaging::UioShmPort::Disconnect
    * @tsd_testexpected nothing happens
    */
   void test_Disconnect_WithConnector_NothingHappens();

   CPPUNIT_TEST_SUITE(UioShmPortTest);
   CPPUNIT_TEST(test_Constructor_WithRouter_ObjectCreated);
   CPPUNIT_TEST(test_ConnectUpstream_MemberConnector_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ConnectUpstream_NotMemberConnectorAndMemberListener_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ConnectUpstream_NotMemberConnectorAndNotMemberListener_NothingHappens);
   CPPUNIT_TEST(test_ListenDownstream_MemberListener_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ListenDownstream_NotMemberListenerAndMemberConnector_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ListenDownstream_NotMemberListenerAndNotMemberConnector_NothingHappens);
   CPPUNIT_TEST(test_Disconnect_WithoutListenerAndConnector_NothingHappens);
   CPPUNIT_TEST(test_Disconnect_WithListener_NothingHappens);
   CPPUNIT_TEST(test_Disconnect_WithConnector_NothingHappens);
   CPPUNIT_TEST_SUITE_END();

private:
   std::shared_ptr<UioShmPort> m_TestObj;
   char                        m_fName[16];
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_UIOSHMPORTTEST_HPP
