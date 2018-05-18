//////////////////////////////////////////////////////////////////////
/// @file NameServerTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test NameServer
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "NameServerTest.hpp"
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/NameServer.hpp>
#include <tsd/communication/messaging/NameServerHelper.hpp>
#include <tsd/communication/messaging/Router.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
const std::string DEFAULT_SERVER_NAME = "NameServer";
const uint32_t    DEFAULT_TIMEOUT{1};
const bool        IS_NAME_SERVER{true};
const std::string DEFAULT_INTERFACE_NAME           = "notNullLookupName";
const std::string INTERFACE_NAME_WITH_SLASH        = "/notNullLookupName";
const std::string INTERFACE_NAME_WITH_DOUBLE_SLASH = "/notNullLookupName/";
const std::string EMPTY_INTERFACE_NAME             = "";
const std::string HOST_NAME_NONULLLOOKUPNAME       = "host/name/notNullLookupName";
const std::string HOST_NAME                        = "host/name";
const std::string HOST_NAME_WITH_SLASH             = "/host/name";
}

NameServerTest::~NameServerTest()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void NameServerTest::test_Run_QuitIndCaseTaken_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Run_QueryNameReqCaseTakenAndMemberStubResolver_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE("MakeStubResolver returned false", true, testObj.makeStubResolver(remoteAddr));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, (testObj.lookupInterface(DEFAULT_INTERFACE_NAME, DEFAULT_TIMEOUT, remoteAddr)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Run_QueryNameReqCaseTakenAndNotMemberStubResolver_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, (testObj.lookupInterface(DEFAULT_INTERFACE_NAME, DEFAULT_TIMEOUT, remoteAddr)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Run_QueryNameReqCaseAndInterfaceNameStartingWithSlash_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(INTERFACE_NAME_WITH_SLASH, remoteAddr, IS_NAME_SERVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, (testObj.lookupInterface(INTERFACE_NAME_WITH_SLASH, DEFAULT_TIMEOUT, remoteAddr)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Run_QueryNameReqCaseAndSuccess_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, remoteAddr, IS_NAME_SERVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned true", false, (testObj.lookupInterface(INTERFACE_NAME_WITH_SLASH, DEFAULT_TIMEOUT, remoteAddr)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Run_QueryNameReqCaseAndSuccessAndRedirect_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, remoteAddr, IS_NAME_SERVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, (testObj.lookupInterface(INTERFACE_NAME_WITH_DOUBLE_SLASH, DEFAULT_TIMEOUT, remoteAddr)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Constructor_WithRouter_ObjectCreated()
{
   Router                      router(DEFAULT_SERVER_NAME);
   std::shared_ptr<NameServer> nameServer;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", nameServer = std::make_shared<NameServer>(router));
}

void NameServerTest::test_Init_JustRun_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_Fini_JustRun_NothingHappens()
{
   Router     router(DEFAULT_SERVER_NAME);
   NameServer testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
}

void NameServerTest::test_GetInterfaceAddr_JustRun_LocalIfcAddrReturned()
{
   Router     router(DEFAULT_SERVER_NAME);
   NameServer testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.getInterfaceAddr());
}

void NameServerTest::test_PublishInterface_NotStubResolver_TrueReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, address, IS_NAME_SERVER));
}

void NameServerTest::test_PublishInterface_StubResolverAndNotRet_FalseReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};
   tsd::communication::event::IfcAddr_t zeroAddress{0};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, address, IS_NAME_SERVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(zeroAddress));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned true", false, (testObj.publishInterface(DEFAULT_INTERFACE_NAME, address, IS_NAME_SERVER)));
}

void NameServerTest::test_PublishInterface_WithRunningThreadAndStubResolverSet_TrueReturned()
{
   //   this test would cover remaining branches in run, publishInterface and publishInterfaceUpstream methods
   //   however it's impossible to execute due to bug SUPMOB-1568

   //   Router router(DEFAULT_SERVER_NAME);
   //   NameServerHelper testObj(router);
   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", (testObj.init()));
   //   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   //   tsd::communication::event::IfcAddr_t address{1};

   //   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(testObj.getInterfaceAddr()));
   //   CPPUNIT_ASSERT_EQUAL_MESSAGE("Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, address,
   //   IS_NAME_SERVER));
   //   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   //   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_UnpublishInterface_MemberStubResolver_NothingHappens()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};
   tsd::communication::event::IfcAddr_t zeroAddress{0};

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(zeroAddress));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
                                   testObj.unpublishInterface(DEFAULT_INTERFACE_NAME, address));
}

void NameServerTest::test_UnpublishInterface_NotMemberStubResolver_NothingHappens()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, address, IS_NAME_SERVER));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
                                   testObj.unpublishInterface(DEFAULT_INTERFACE_NAME, address));
}

void NameServerTest::test_UnpublishInterface_MemberStubResolverAndWhileRunning_NothingHappens()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", (testObj.init()));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t address{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(testObj.getInterfaceAddr()));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
                                   testObj.unpublishInterface(DEFAULT_INTERFACE_NAME, address));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_LookupInterface_InterfaceNameEmpty_FalseReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t remoteAddr{1};
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, testObj.lookupInterface(EMPTY_INTERFACE_NAME, DEFAULT_TIMEOUT, remoteAddr));
}

void NameServerTest::test_LookupInterface_ReturningNullMessage_FalseReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t remoteAddr{1};
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, testObj.lookupInterface(DEFAULT_INTERFACE_NAME, DEFAULT_TIMEOUT, remoteAddr));
}

void NameServerTest::test_LookupInterface_WithRunningThreadAndManyEntriesInAuthDomainVec_FalseReturned()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", (testObj.init()));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Make authorative returned false", true, testObj.makeAuthorative(testObj.getInterfaceAddr(), HOST_NAME_NONULLLOOKUPNAME));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, testObj.lookupInterface(INTERFACE_NAME_WITH_SLASH, DEFAULT_TIMEOUT, remoteAddr));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_LookupInterface_WithRunningThreadAndInterfaceNameHasHostName_FalseReturned()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());
   tsd::communication::event::IfcAddr_t remoteAddr{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make authorative returned false", true, testObj.makeAuthorative(testObj.getInterfaceAddr(), HOST_NAME));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Lookup interface returned true", false, testObj.lookupInterface(HOST_NAME_WITH_SLASH, DEFAULT_TIMEOUT, remoteAddr));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_MakeLocal_JustRun_NothingHappens()
{
   Router     router(DEFAULT_SERVER_NAME);
   NameServer testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.makeLocal());
}

void NameServerTest::test_MakeStubResolver_MemberStubResolver_FalseReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(address));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned true", false, testObj.makeStubResolver(address));
}

void NameServerTest::test_MakeStubResolver_NotMemberStubResolverAndMemberUpstreamNameServerEqualsLoopbackAddress_TrueReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(address));
}

void NameServerTest::test_MakeAuthorative_MemberStubResolver_FalseReturned()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t address{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(address));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make authorative returned true", false, testObj.makeAuthorative(address, DEFAULT_INTERFACE_NAME));
}

void NameServerTest::test_MakeAuthorative_WithRunningThread_TrueReturned()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Make authorative returned false", true, testObj.makeAuthorative(testObj.getInterfaceAddr(), DEFAULT_INTERFACE_NAME));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_MakeAuthorative_WithRunningThreadAndAlreadyPublishedInterface_TrueReturned()
{
   Router           router(DEFAULT_SERVER_NAME);
   NameServerHelper testObj(router);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.init());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not started", true, testObj.isRunning());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Publish interface returned false",
                                true,
                                testObj.publishInterface(DEFAULT_INTERFACE_NAME, testObj.getInterfaceAddr(), IS_NAME_SERVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Make authorative retuned true", false, testObj.makeAuthorative(testObj.getInterfaceAddr(), DEFAULT_INTERFACE_NAME));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.fini());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread was not stopped", false, testObj.isRunning());
}

void NameServerTest::test_PortVanished_IsNetworkAddrAndMemberStubResolver_NothingHappens()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t netaddr{1};
   tsd::communication::event::IfcAddr_t netmask{1};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, netaddr, IS_NAME_SERVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Make stub resolver returned false", true, testObj.makeStubResolver(netaddr));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.portVanished(netaddr, netmask));
}

void NameServerTest::test_PortVanished_NotIsNetworkAddrAndNotMemberStubResolver_NothingHappens()
{
   Router                               router(DEFAULT_SERVER_NAME);
   NameServer                           testObj(router);
   tsd::communication::event::IfcAddr_t addres{1};
   tsd::communication::event::IfcAddr_t netaddr{2};
   tsd::communication::event::IfcAddr_t netmask{3};

   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Publish interface returned false", true, testObj.publishInterface(DEFAULT_INTERFACE_NAME, addres, IS_NAME_SERVER));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", testObj.portVanished(netaddr, netmask));
}

CPPUNIT_TEST_SUITE_REGISTRATION(NameServerTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
