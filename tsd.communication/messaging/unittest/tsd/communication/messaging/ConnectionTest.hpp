//////////////////////////////////////////////////////////////////////
/// @file ConnectionTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test Connection
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTIONTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTIONTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

class ConnectionHelper;

/**
 * Testclass for Connection
 *
 * @brief Testclass for Connection
 */
class ConnectionTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;

   void tearDown() override;

   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::SetConnected
    * @tsd_testexpected nothing happens
    */
   void test_SetConnected_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::SetDisconnected
    * @tsd_testexpected nothing happens
    */
   void test_SetDisconnected_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::ProcessData
    * @tsd_testexpected nothing happens
    */
   void test_ProcessData_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::GetData
    * @tsd_testexpected member p get data returned
    */
   void test_GetData_JustRun_MemberPGetDataReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::ConsumeData
    * @tsd_testexpected nothing happens
    */
   void test_ConsumeData_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::ConnectUpstream
    * @tsd_testexpected member p connect upstream returned
    */
   void test_ConnectUpstream_JustRun_MemberPConnectUpstreamReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::ListenDownstream
    * @tsd_testexpected member p listen downstream returned
    */
   void test_ListenDownstream_JustRun_MemberPListenDownstreamReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Connection::Disconnect
    * @tsd_testexpected nothing happens
    */
   void test_Disconnect_JustRun_NothingHappens();

private:
   CPPUNIT_TEST_SUITE(ConnectionTest);
   CPPUNIT_TEST(test_SetConnected_JustRun_NothingHappens);
   CPPUNIT_TEST(test_SetDisconnected_JustRun_NothingHappens);
   CPPUNIT_TEST(test_ProcessData_JustRun_NothingHappens);
   CPPUNIT_TEST(test_GetData_JustRun_MemberPGetDataReturned);
   CPPUNIT_TEST(test_ConsumeData_JustRun_NothingHappens);
   CPPUNIT_TEST(test_ConnectUpstream_JustRun_MemberPConnectUpstreamReturned);
   CPPUNIT_TEST(test_ListenDownstream_JustRun_MemberPListenDownstreamReturned);
   CPPUNIT_TEST(test_Disconnect_JustRun_NothingHappens);
   CPPUNIT_TEST_SUITE_END();

   std::unique_ptr<ConnectionHelper> m_TestObj;
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_CONNECTIONTEST_HPP
