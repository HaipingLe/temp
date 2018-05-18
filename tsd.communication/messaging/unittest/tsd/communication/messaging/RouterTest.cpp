//////////////////////////////////////////////////////////////////////
/// @file RouterTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test Router
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#define _GLIBCXX_TR1_TUPLE 0
#define _GLIBCXX_TR1_FUNCTIONAL 0

#include "RouterTest.hpp"
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/IPortMock.hpp>
#include <tsd/communication/messaging/Packet.hpp>
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>

namespace tsd {
namespace communication {
namespace messaging {

void RouterTest::setUp()
{
   m_TestObjectName = "testName";
   m_TestObj        = std::make_shared<Router>(m_TestObjectName);
}

void RouterTest::tearDown()
{
   Router::cleanup();
   tsd::common::logging::LoggingManager::cleanup();
}

void RouterTest::test_Constructor_CreateWithName_ExpectingObjectCreatedAndMembersSet()
{
   std::string             testName{"testName"};
   std::unique_ptr<Router> testRouter;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor failed with a throw", testRouter = std::unique_ptr<Router>{new Router(testName)});
   CPPUNIT_ASSERT_MESSAGE("Object null after creating with constructor", nullptr != testRouter);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Name in router doesn't match with one expected", testRouter->getName(), testName);
}

void RouterTest::test_GetLocalRouter_InvokeWhenLocalRouterNull_ExpectingRouterCreatedWithSpecificName()
{
   std::string expName{"localhost"};
   Router&     retRouter = m_TestObj->getLocalRouter();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Router has different name then expected after getLocalRouter", retRouter.getName(), expName);
}

void RouterTest::test_GetLocalRouter_InvokeWhenLocalRouterNotNull_ExpectingRouterReturnedWithSameAsFirstReference()
{
   std::string expName{"localhost"};
   Router&     retRouter = m_TestObj->getLocalRouter();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Router has different name then expected after getLocalRouter", retRouter.getName(), expName);
   Router& secRouter = m_TestObj->getLocalRouter();
   CPPUNIT_ASSERT_EQUAL_MESSAGE("local router changed reference when unexpectred", &retRouter, &secRouter);
}

void RouterTest::test_AllocateIfcAddr_InvokeProvidedQueue_ExpectingSpecificAddressReturned()
{
   std::shared_ptr<tsd::communication::messaging::Queue> testQueue{
      std::make_shared<tsd::communication::messaging::Queue>("testQueue", *m_TestObj.get())};
   tsd::communication::event::IfcAddr_t retAddr = m_TestObj->allocateIfcAddr(testQueue.get());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Address returned by allocation doesn't match expected", uint64_t(3), retAddr);
}

void RouterTest::test_FreeIfcAddr_InvokeProvidedAddressNumber_ExpectingNoThrowsNotFurtherTestable()
{
   std::shared_ptr<tsd::communication::messaging::Queue> testQueue{
      std::make_shared<tsd::communication::messaging::Queue>("testQueue", *m_TestObj.get())};
   tsd::communication::event::IfcAddr_t retAddr = m_TestObj->allocateIfcAddr(testQueue.get());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("freeIfcAddr failed with a throw", m_TestObj->freeIfcAddr(retAddr));
}

void RouterTest::test_SetSubnetConfig_InvokeWhenSubNetsEmpty_ExpectingTrueReturned()
{
   uint8_t testPrefixLen{2};
   bool    response = m_TestObj->setSubnetConfig(testPrefixLen);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("False returned from setSubnetConfig unexpectedly", true, response);
}

void RouterTest::test_SetSubnetConfig_InvokeWhenSubNetsNotEmpty_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1}, testNameServAddr{2};
   uint8_t                              testPrefix{0};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   m_TestObj->allocateDownstreamAddr(testAddr, testPrefix, testNameServAddr);
   bool response = m_TestObj->setSubnetConfig(testPrefix);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("True returned from setSubnetConfig unexpectedly", false, response);
}

void RouterTest::test_GetNameServer_JustRun_ExpectingServersAddressReturned()
{
   tsd::communication::event::IfcAddr_t retAddr = m_TestObj->getNameServer();
   CPPUNIT_ASSERT_MESSAGE("getNameServer returned invalid address", uint64_t{0} != retAddr);
}

void RouterTest::test_AllocateDownstreamAddr_InvokeWhenSubNetPrefixLengthIs0_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1}, testNameServAddr{2};
   uint8_t                              testPrefix{0};
   m_TestObj->setSubnetConfig(testPrefix);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("allocateDownstreamAddr returned true when false expected",
                                false,
                                m_TestObj->allocateDownstreamAddr(testAddr, testPrefix, testNameServAddr));
}

void RouterTest::test_AllocateDownstreamAddr_DefaultGatewayNull_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1}, testNameServAddr{2};
   uint8_t                              testPrefix{0};
   CPPUNIT_ASSERT_EQUAL_MESSAGE("allocateDownstreamAddr returned false when true expected",
                                true,
                                m_TestObj->allocateDownstreamAddr(testAddr, testPrefix, testNameServAddr));
}

void RouterTest::test_AllocateDownstreamAddr_DefaultGatewayNotNull_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1}, testNameServAddr{2};
   uint8_t                              testPrefix{0};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("allocateDownstreamAddr returned false when true expected",
                                true,
                                m_TestObj->allocateDownstreamAddr(testAddr, testPrefix, testNameServAddr));
}

void RouterTest::test_FreeDownstreamAddr_InvokeAfterAddingAddress_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr{1}, testNameServAddr{2};
   uint8_t                              testPrefix{0};
   m_TestObj->allocateDownstreamAddr(testAddr, testPrefix, testNameServAddr);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("freeDownStream failed with a throw", m_TestObj->freeDownstreamAddr(testAddr));
}

void RouterTest::test_AddDownstreamPort_InvokeProvidedPort_ExpectingNoThrows()
{
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   m_TestObj->addDownstreamPort(testPort.get());
}

void RouterTest::test_AddUpstreamPort_InvokeWhenDefaultGatewayNullSubNetsNotEmptySubDomainEmpty_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testNameServAddr{1};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "addUpstreamPort returned false when true expected", true, m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, ""));
}

void RouterTest::test_AddUpstreamPort_InvokeWhenDefaultGatewayNullSubNetsNotEmptySubDomainNotEmpty_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testNameServAddr{1};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   UNUSED(testNameServAddr);
   UNUSED(testPort);
   // Below case enters infinite loop. Bug reported SUPMOB-1552
   // CPPUNIT_ASSERT_EQUAL_MESSAGE("addUpstreamPort returned false when true expected",
   //                             true, m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "test"));
}

void RouterTest::test_AddUpstreamPort_InvokeWhenDefaultGatewayNotNull_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testNameServAddr{1};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "addUpstreamPort returned false when true expected", false, m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, ""));
}

void RouterTest::test_AddUpstreamPort_InvokeWhenSubNetsNotEmpty_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1}, testNameServAddr{2};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   uint8_t                              testPrefix{0};
   m_TestObj->allocateDownstreamAddr(testAddr, testPrefix, testNameServAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "addUpstreamPort returned false when true expected", false, m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, ""));
}

void RouterTest::test_DelPort_InvokeProvidedPortExistingInPortsAndIsDefaultGateway_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testNameServAddr{1};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("delPort failed with a throw", m_TestObj->delPort(testPort.get()));
}

void RouterTest::test_DelPort_InvokeProvidedPortNotExistingInPortsAndNotDefaultGateway_ExpectingNoThrows()
{
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("delPort failed with a throw", m_TestObj->delPort(testPort.get()));
}

void RouterTest::test_IsAnyPortAddr_InvokeProvidedPortSameToDefaultGateway_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testNameServAddr{1};
   IPortMock*                           testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "isAnyPortAddr returned false when true expected", true, m_TestObj->isAnyPortAddr(testPort->getLocalAddr()));
}

void RouterTest::test_IsAnyPortAddr_InvokeProvidedPortExistingInPortsDefaultGatewayNull_ExpectingTrueReturned()
{
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   m_TestObj->addDownstreamPort(testPort.get());
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "isAnyPortAddr returned false when true expected", true, m_TestObj->isAnyPortAddr(testPort->getLocalAddr()));
}
// This is going to return true as default address is the same as localhost address
void RouterTest::test_IsAnyPortAddr_InvokeProvidedPortNotDefaultGateway_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testNameServAddr{1};
   IPortMock*                           testPortMock1 = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort1(testPortMock1);
   IPortMock*                           testPortMock2 = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort>               testPort2(testPortMock2);
   m_TestObj->addUpstreamPort(testPort1.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "isAnyPortAddr returned false when true expected", true, m_TestObj->isAnyPortAddr(testPort2->getLocalAddr()));
}

void RouterTest::test_PublishInterface_JustInvoke_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1};
   CPPUNIT_ASSERT_EQUAL_MESSAGE("publish interface returned false unexpectedly", true, m_TestObj->publishInterface("", testAddr));
}

void RouterTest::test_UnpublishInterface_JustInvoke_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr{1};
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("unpublishInterface failed with a throw", m_TestObj->unpublishInterface("", testAddr));
}

void RouterTest::test_LookupInterface_InvokeProvidedEmptyInterfaceName_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr{1};
   uint32_t                             testTimeout{1};
   CPPUNIT_ASSERT_EQUAL_MESSAGE("lookupInterface returned true unexpectedly", false, m_TestObj->lookupInterface("", testTimeout, testAddr));
}

void RouterTest::test_JoinGroup_InvokeProvidedLocalSenderAddress_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0}, testReceiverAddr{2};
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "joinGroup Alive returned true unexpectedly", false, m_TestObj->joinGroup(testSenderAddr, testReceiverAddr));
}

void RouterTest::test_JoinGroup_InvokeProvidedNonLocalSenderAddressExistingInStubs_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{2};
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "joinGroup Alive returned true unexpectedly", false, m_TestObj->joinGroup(testSenderAddr, testReceiverAddr));
}

void RouterTest::test_JoinGroup_InvokeProvidedNonLocalSenderAddressNotInStubs_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{2};
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "joinGroup Alive returned true unexpectedly", false, m_TestObj->joinGroup(testSenderAddr, testReceiverAddr));
}

void RouterTest::test_LeaveGroup_InvokeProvidedLocalSenderAndMatchingReceiver_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0}, testReceiverAddr{2};
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("leaveGroup returned false unexpected", true, m_TestObj->leaveGroup(testSenderAddr, testReceiverAddr));
}

void RouterTest::test_LeaveGroup_InvokeProvidedLocalSenderAndNotMatchingReceiver_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0}, testReceiverAddr{2}, testReceiverToLeave{0xffffffff};
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("leaveGroup returned true unexpected", false, m_TestObj->leaveGroup(testSenderAddr, testReceiverToLeave));
}

void RouterTest::test_LeaveGroup_InvokeProvidedNonLocalSenderAndMatchingReceiver_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{2};
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("leaveGroup returned true unexpected", true, m_TestObj->leaveGroup(testSenderAddr, testReceiverAddr));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("leaveGroup returned false unexpected", false, m_TestObj->leaveGroup(testSenderAddr, testReceiverAddr));
}

void RouterTest::test_LeaveGroup_InvokeProvidedNonLocalSenderAndNotMatchingReceiver_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{2}, testReceiverToLeave{0xffffffff};
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("leaveGroup returned false unexpected", false, m_TestObj->leaveGroup(testSenderAddr, testReceiverToLeave));
}

void RouterTest::test_LeaveGroup_InvokeProvidedNonLocalSenderNotInStubsAndMatchingReceiver_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testSenderDiffAddr{0xffffff00}, testReceiverAddr{2};
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("leaveGroup returned false unexpected", false, m_TestObj->leaveGroup(testSenderDiffAddr, testReceiverAddr));
}

void RouterTest::test_RoutePacket_InvokeWhenEgressPortNullIngressNotNullPacketTypeMulticastJoin_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{0xffffffff}, testNameServAddr{2};
   m_TestPacket.reset(new Packet(Packet::MULTICAST_JOIN, testSenderAddr, testReceiverAddr));
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned false unexpectedly", true, m_TestObj->routePacket(m_TestPacket, testPort.get()));
}

void RouterTest::test_RoutePacket_InvokeWhenEgressPortNullIngressPortNotNullPacketTypeMulticastLeave_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{0xffffff00}, testNameServAddr{2};
   m_TestPacket.reset(new Packet(Packet::MULTICAST_LEAVE, testSenderAddr, testReceiverAddr));
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned false unexpectedly", true, m_TestObj->routePacket(m_TestPacket, testPort.get()));
}

void RouterTest::test_RoutePacket_InvokeWhenEgressPortNullIngressPortNotNullPacketTypeDeathNotification_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{0xffffff00}, testNameServAddr{2};
   m_TestPacket.reset(new Packet(Packet::DEATH_NOTIFICATION, testSenderAddr, testReceiverAddr));
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned false unexpectedly", true, m_TestObj->routePacket(m_TestPacket, testPort.get()));
}

void RouterTest::test_RoutePacket_InvokeWhenEgressPortNullIngressPortNullPacketTypeUnicastMessage_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{0};
   m_TestPacket.reset(new Packet(Packet::UNICAST_MESSAGE, testSenderAddr, testReceiverAddr));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned true unexpectedly", false, m_TestObj->routePacket(m_TestPacket, nullptr));
}

void RouterTest::test_RoutePacket_InvokeWhenEgressPortNullIngressPortNullPacketTypeMulticastMessageGroupNotExisting_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{0};
   m_TestPacket.reset(new Packet(Packet::MULTICAST_MESSAGE, testSenderAddr, testReceiverAddr));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned true unexpectedly", false, m_TestObj->routePacket(m_TestPacket, nullptr));
}

void RouterTest::test_RoutePacket_InvokeWhenPacketTypeMulticastMessageGroupExistingFoundEqualLoopBackAddress_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0}, testReceiverAddr{0};
   m_TestPacket.reset(new Packet(Packet::MULTICAST_MESSAGE, testSenderAddr, testReceiverAddr));
   m_TestObj->joinGroup(testReceiverAddr, testSenderAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned true unexpectedly", false, m_TestObj->routePacket(m_TestPacket, nullptr));
}

void RouterTest::test_RoutePacket_InvokeWhenEgressPortNullIngressNotNullPacketTypeDhcpOffer_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{0xffffffff}, testNameServAddr{2};
   m_TestPacket.reset(new Packet(Packet::DHCP_OFFER, testSenderAddr, testReceiverAddr));
   IPortMock*             testPortMock = new IPortMock(*m_TestObj.get());
   std::shared_ptr<IPort> testPort(testPortMock);
   m_TestObj->addUpstreamPort(testPort.get(), testNameServAddr, "");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("routePacket returned true unexpectedly", false, m_TestObj->routePacket(m_TestPacket, testPort.get()));
}

void RouterTest::test_SendUnicastMessage_InvokeProvidedSenderReceiverAndMessage_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0xffffffff}, testReceiverAddr{2};
   m_TestEvent.reset(new tsd::communication::event::TsdEvent(uint32_t{0}));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendUnicastMessage failed with a throw",
                                   m_TestObj->sendUnicastMessage(testSenderAddr, testReceiverAddr, m_TestEvent));
}

void RouterTest::test_SendBroadcastMessage_InvokeWhenGroupExistingAndFoundIsNotLocalAddress_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0}, testReceiverAddr1{0xffffffff}, testReceiverAddr2{0xffffffff};
   m_TestEvent.reset(new tsd::communication::event::TsdEvent(uint32_t{0}));
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr1);
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr2);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendBroadcastMessage failed with a throw",
                                   m_TestObj->sendBroadcastMessage(testSenderAddr, m_TestEvent));
}

void RouterTest::test_SendBroadcastMessage_InvokeWhenGroupExistingAndFoundIsLocalAddress_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0}, testReceiverAddr{0};
   m_TestEvent.reset(new tsd::communication::event::TsdEvent(uint32_t{0}));
   m_TestObj->joinGroup(testSenderAddr, testReceiverAddr);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendBroadcastMessage failed with a throw",
                                   m_TestObj->sendBroadcastMessage(testSenderAddr, m_TestEvent));
}

void RouterTest::test_SendBroadcastMessage_InvokeWhenGroupNotExisting_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testSenderAddr{0};
   m_TestEvent.reset(new tsd::communication::event::TsdEvent(uint32_t{0}));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendBroadcastMessage failed with a throw",
                                   m_TestObj->sendBroadcastMessage(testSenderAddr, m_TestEvent));
}

void RouterTest::test_GetName_JustInvoke_ExpectingNameProvidedInConstructor()
{
   CPPUNIT_ASSERT_EQUAL_MESSAGE("getName returned different name than expected", m_TestObjectName, m_TestObj->getName());
}

CPPUNIT_TEST_SUITE_REGISTRATION(RouterTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
