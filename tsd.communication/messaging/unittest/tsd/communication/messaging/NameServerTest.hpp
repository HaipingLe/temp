//////////////////////////////////////////////////////////////////////
/// @file NameServerTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test NameServer
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_NAMESERVERTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_NAMESERVERTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

class NameServer;
class Router;

/**
 * Testclass for NameServer
 *
 * @brief Testclass for NameServer
 */
class NameServerTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Destructor to clear singletons.
    */
   virtual ~NameServerTest();

   /**
    * @brief Test scenario: quit ind case taken
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Run
    * @tsd_testexpected nothing happens
    */
   void test_Run_QuitIndCaseTaken_NothingHappens();
   /**
    * @brief Test scenario: query name req case taken and member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Run
    * @tsd_testexpected nothing happens
    */
   void test_Run_QueryNameReqCaseTakenAndMemberStubResolver_NothingHappens();
   /**
    * @brief Test scenario: query name req case taken and not member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Run
    * @tsd_testexpected nothing happens
    */
   void test_Run_QueryNameReqCaseTakenAndNotMemberStubResolver_NothingHappens();
   /**
    * @brief Test scenario: query name req case and interface name starting with slash
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Run
    * @tsd_testexpected nothing happens
    */
   void test_Run_QueryNameReqCaseAndInterfaceNameStartingWithSlash_NothingHappens();
   /**
    * @brief Test scenario: query name req case and success
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Run
    * @tsd_testexpected nothing happens
    */
   void test_Run_QueryNameReqCaseAndSuccess_NothingHappens();
   /**
    * @brief Test scenario: query name req case and success and redirect
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Run
    * @tsd_testexpected nothing happens
    */
   void test_Run_QueryNameReqCaseAndSuccessAndRedirect_NothingHappens();
   /**
    * @brief Test scenario: with router
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithRouter_ObjectCreated();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Init
    * @tsd_testexpected nothing happens
    */
   void test_Init_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::Fini
    * @tsd_testexpected nothing happens
    */
   void test_Fini_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::GetInterfaceAddr
    * @tsd_testexpected local ifc addr returned
    */
   void test_GetInterfaceAddr_JustRun_LocalIfcAddrReturned();
   /**
    * @brief Test scenario: not stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::PublishInterface
    * @tsd_testexpected true returned
    */
   void test_PublishInterface_NotStubResolver_TrueReturned();
   /**
    * @brief Test scenario: stub resolver and not ret
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::PublishInterface
    * @tsd_testexpected false returned
    */
   void test_PublishInterface_StubResolverAndNotRet_FalseReturned();
   /**
    * @brief Test scenario: with running thread and stub resolver set
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::PublishInterface
    * @tsd_testexpected true returned
    */
   void test_PublishInterface_WithRunningThreadAndStubResolverSet_TrueReturned();
   /**
    * @brief Test scenario: member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::UnpublishInterface
    * @tsd_testexpected nothing happens
    */
   void test_UnpublishInterface_MemberStubResolver_NothingHappens();
   /**
    * @brief Test scenario: not member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::UnpublishInterface
    * @tsd_testexpected nothing happens
    */
   void test_UnpublishInterface_NotMemberStubResolver_NothingHappens();
   /**
    * @brief Test scenario: member stub resolver and while running
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::UnpublishInterface
    * @tsd_testexpected nothing happens
    */
   void test_UnpublishInterface_MemberStubResolverAndWhileRunning_NothingHappens();
   /**
    * @brief Test scenario: interface name empty
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::LookupInterface
    * @tsd_testexpected false returned
    */
   void test_LookupInterface_InterfaceNameEmpty_FalseReturned();
   /**
    * @brief Test scenario: returning null message
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::LookupInterface
    * @tsd_testexpected false returned
    */
   void test_LookupInterface_ReturningNullMessage_FalseReturned();
   /**
    * @brief Test scenario: with running thread and many entries in auth domain vec
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::LookupInterface
    * @tsd_testexpected false returned
    */
   void test_LookupInterface_WithRunningThreadAndManyEntriesInAuthDomainVec_FalseReturned();
   /**
    * @brief Test scenario: with running thread and interface name has host name
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::LookupInterface
    * @tsd_testexpected false returned
    */
   void test_LookupInterface_WithRunningThreadAndInterfaceNameHasHostName_FalseReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::MakeLocal
    * @tsd_testexpected nothing happens
    */
   void test_MakeLocal_JustRun_NothingHappens();
   /**
    * @brief Test scenario: member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::MakeStubResolver
    * @tsd_testexpected false returned
    */
   void test_MakeStubResolver_MemberStubResolver_FalseReturned();
   /**
    * @brief Test scenario: not member stub resolver and member upstream name server equals loopback address
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::MakeStubResolver
    * @tsd_testexpected true returned
    */
   void test_MakeStubResolver_NotMemberStubResolverAndMemberUpstreamNameServerEqualsLoopbackAddress_TrueReturned();
   /**
    * @brief Test scenario: member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::MakeAuthorative
    * @tsd_testexpected false returned
    */
   void test_MakeAuthorative_MemberStubResolver_FalseReturned();
   /**
    * @brief Test scenario: with running thread
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::MakeAuthorative
    * @tsd_testexpected true returned
    */
   void test_MakeAuthorative_WithRunningThread_TrueReturned();
   /**
    * @brief Test scenario: with running thread and already published interface
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::MakeAuthorative
    * @tsd_testexpected true returned
    */
   void test_MakeAuthorative_WithRunningThreadAndAlreadyPublishedInterface_TrueReturned();
   /**
    * @brief Test scenario: is network addr and member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::PortVanished
    * @tsd_testexpected nothing happens
    */
   void test_PortVanished_IsNetworkAddrAndMemberStubResolver_NothingHappens();
   /**
    * @brief Test scenario: not is network addr and not member stub resolver
    *
    * @tsd_testobject tsd::communication::messaging::NameServer::PortVanished
    * @tsd_testexpected nothing happens
    */
   void test_PortVanished_NotIsNetworkAddrAndNotMemberStubResolver_NothingHappens();

   CPPUNIT_TEST_SUITE(NameServerTest);
   CPPUNIT_TEST(test_Run_QuitIndCaseTaken_NothingHappens);
   CPPUNIT_TEST(test_Run_QueryNameReqCaseTakenAndMemberStubResolver_NothingHappens);
   CPPUNIT_TEST(test_Run_QueryNameReqCaseTakenAndNotMemberStubResolver_NothingHappens);
   CPPUNIT_TEST(test_Run_QueryNameReqCaseAndInterfaceNameStartingWithSlash_NothingHappens);
   CPPUNIT_TEST(test_Run_QueryNameReqCaseAndSuccess_NothingHappens);
   CPPUNIT_TEST(test_Run_QueryNameReqCaseAndSuccessAndRedirect_NothingHappens);
   CPPUNIT_TEST(test_Constructor_WithRouter_ObjectCreated);
   CPPUNIT_TEST(test_Init_JustRun_NothingHappens);
   CPPUNIT_TEST(test_Fini_JustRun_NothingHappens);
   CPPUNIT_TEST(test_GetInterfaceAddr_JustRun_LocalIfcAddrReturned);
   CPPUNIT_TEST(test_PublishInterface_NotStubResolver_TrueReturned);
   CPPUNIT_TEST(test_PublishInterface_StubResolverAndNotRet_FalseReturned);
   CPPUNIT_TEST(test_PublishInterface_WithRunningThreadAndStubResolverSet_TrueReturned);
   CPPUNIT_TEST(test_UnpublishInterface_MemberStubResolver_NothingHappens);
   CPPUNIT_TEST(test_UnpublishInterface_NotMemberStubResolver_NothingHappens);
   CPPUNIT_TEST(test_UnpublishInterface_MemberStubResolverAndWhileRunning_NothingHappens);
   CPPUNIT_TEST(test_LookupInterface_InterfaceNameEmpty_FalseReturned);
   CPPUNIT_TEST(test_LookupInterface_ReturningNullMessage_FalseReturned);
   CPPUNIT_TEST(test_LookupInterface_WithRunningThreadAndManyEntriesInAuthDomainVec_FalseReturned);
   CPPUNIT_TEST(test_LookupInterface_WithRunningThreadAndInterfaceNameHasHostName_FalseReturned);
   CPPUNIT_TEST(test_MakeLocal_JustRun_NothingHappens);
   CPPUNIT_TEST(test_MakeStubResolver_MemberStubResolver_FalseReturned);
   CPPUNIT_TEST(test_MakeStubResolver_NotMemberStubResolverAndMemberUpstreamNameServerEqualsLoopbackAddress_TrueReturned);
   CPPUNIT_TEST(test_MakeAuthorative_MemberStubResolver_FalseReturned);
   CPPUNIT_TEST(test_MakeAuthorative_WithRunningThread_TrueReturned);
   CPPUNIT_TEST(test_MakeAuthorative_WithRunningThreadAndAlreadyPublishedInterface_TrueReturned);
   CPPUNIT_TEST(test_PortVanished_IsNetworkAddrAndMemberStubResolver_NothingHappens);
   CPPUNIT_TEST(test_PortVanished_NotIsNetworkAddrAndNotMemberStubResolver_NothingHappens);
   CPPUNIT_TEST_SUITE_END();

private:
   std::shared_ptr<NameServer> m_TestObj;
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_NAMESERVERTEST_HPP
