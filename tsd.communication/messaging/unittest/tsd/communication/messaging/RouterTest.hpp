//////////////////////////////////////////////////////////////////////
/// @file RouterTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test Router
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_ROUTERTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_ROUTERTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace event {

class TsdEvent;
} // namespace event

namespace messaging {

class Router;
class Packet;

/**
 * Testclass for Router
 *
 * @brief Testclass for Router
 */
class RouterTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;

   void tearDown();

   /**
    * @brief Test scenario: create with name
    *
    * @tsd_testobject tsd::communication::messaging::Router::Constructor
    * @tsd_testexpected expecting object created and members set
    */
   void test_Constructor_CreateWithName_ExpectingObjectCreatedAndMembersSet();
   /**
    * @brief Test scenario: invoke when local router null
    *
    * @tsd_testobject tsd::communication::messaging::Router::GetLocalRouter
    * @tsd_testexpected expecting router created with specific name
    */
   void test_GetLocalRouter_InvokeWhenLocalRouterNull_ExpectingRouterCreatedWithSpecificName();
   /**
    * @brief Test scenario: invoke when local router not null
    *
    * @tsd_testobject tsd::communication::messaging::Router::GetLocalRouter
    * @tsd_testexpected expecting router returned with same as first reference
    */
   void test_GetLocalRouter_InvokeWhenLocalRouterNotNull_ExpectingRouterReturnedWithSameAsFirstReference();
   /**
    * @brief Test scenario: invoke provided queue
    *
    * @tsd_testobject tsd::communication::messaging::Router::AllocateIfcAddr
    * @tsd_testexpected expecting specific address returned
    */
   void test_AllocateIfcAddr_InvokeProvidedQueue_ExpectingSpecificAddressReturned();
   /**
    * @brief Test scenario: invoke provided address number
    *
    * @tsd_testobject tsd::communication::messaging::Router::FreeIfcAddr
    * @tsd_testexpected expecting no throws not further testable
    */
   void test_FreeIfcAddr_InvokeProvidedAddressNumber_ExpectingNoThrowsNotFurtherTestable();
   /**
    * @brief Test scenario: invoke when sub nets empty
    *
    * @tsd_testobject tsd::communication::messaging::Router::SetSubnetConfig
    * @tsd_testexpected expecting true returned
    */
   void test_SetSubnetConfig_InvokeWhenSubNetsEmpty_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when sub nets not empty
    *
    * @tsd_testobject tsd::communication::messaging::Router::SetSubnetConfig
    * @tsd_testexpected expecting false returned
    */
   void test_SetSubnetConfig_InvokeWhenSubNetsNotEmpty_ExpectingFalseReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Router::GetNameServer
    * @tsd_testexpected expecting servers address returned
    */
   void test_GetNameServer_JustRun_ExpectingServersAddressReturned();
   /**
    * @brief Test scenario: invoke when sub net prefix length is0
    *
    * @tsd_testobject tsd::communication::messaging::Router::AllocateDownstreamAddr
    * @tsd_testexpected expecting false returned
    */
   void test_AllocateDownstreamAddr_InvokeWhenSubNetPrefixLengthIs0_ExpectingFalseReturned();
   /**
    * @brief Test scenario: default gateway null
    *
    * @tsd_testobject tsd::communication::messaging::Router::AllocateDownstreamAddr
    * @tsd_testexpected expecting true returned
    */
   void test_AllocateDownstreamAddr_DefaultGatewayNull_ExpectingTrueReturned();
   /**
    * @brief Test scenario: default gateway not null
    *
    * @tsd_testobject tsd::communication::messaging::Router::AllocateDownstreamAddr
    * @tsd_testexpected expecting true returned
    */
   void test_AllocateDownstreamAddr_DefaultGatewayNotNull_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke after adding address
    *
    * @tsd_testobject tsd::communication::messaging::Router::FreeDownstreamAddr
    * @tsd_testexpected expecting no throws
    */
   void test_FreeDownstreamAddr_InvokeAfterAddingAddress_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke provided port
    *
    * @tsd_testobject tsd::communication::messaging::Router::AddDownstreamPort
    * @tsd_testexpected expecting no throws
    */
   void test_AddDownstreamPort_InvokeProvidedPort_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when default gateway null sub nets not empty sub domain empty
    *
    * @tsd_testobject tsd::communication::messaging::Router::AddUpstreamPort
    * @tsd_testexpected expecting true returned
    */
   void test_AddUpstreamPort_InvokeWhenDefaultGatewayNullSubNetsNotEmptySubDomainEmpty_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when default gateway null sub nets not empty sub domain not empty
    *
    * @tsd_testobject tsd::communication::messaging::Router::AddUpstreamPort
    * @tsd_testexpected expecting true returned
    */
   void test_AddUpstreamPort_InvokeWhenDefaultGatewayNullSubNetsNotEmptySubDomainNotEmpty_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when default gateway not null
    *
    * @tsd_testobject tsd::communication::messaging::Router::AddUpstreamPort
    * @tsd_testexpected expecting false returned
    */
   void test_AddUpstreamPort_InvokeWhenDefaultGatewayNotNull_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke when sub nets not empty
    *
    * @tsd_testobject tsd::communication::messaging::Router::AddUpstreamPort
    * @tsd_testexpected expecting false returned
    */
   void test_AddUpstreamPort_InvokeWhenSubNetsNotEmpty_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided port existing in ports and is default gateway
    *
    * @tsd_testobject tsd::communication::messaging::Router::DelPort
    * @tsd_testexpected expecting no throws
    */
   void test_DelPort_InvokeProvidedPortExistingInPortsAndIsDefaultGateway_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke provided port not existing in ports and not default gateway
    *
    * @tsd_testobject tsd::communication::messaging::Router::DelPort
    * @tsd_testexpected expecting no throws
    */
   void test_DelPort_InvokeProvidedPortNotExistingInPortsAndNotDefaultGateway_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke provided port same to default gateway
    *
    * @tsd_testobject tsd::communication::messaging::Router::IsAnyPortAddr
    * @tsd_testexpected expecting true returned
    */
   void test_IsAnyPortAddr_InvokeProvidedPortSameToDefaultGateway_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke provided port existing in ports default gateway null
    *
    * @tsd_testobject tsd::communication::messaging::Router::IsAnyPortAddr
    * @tsd_testexpected expecting true returned
    */
   void test_IsAnyPortAddr_InvokeProvidedPortExistingInPortsDefaultGatewayNull_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke provided port not default gateway
    *
    * @tsd_testobject tsd::communication::messaging::Router::IsAnyPortAddr
    * @tsd_testexpected expecting true returned
    */
   void test_IsAnyPortAddr_InvokeProvidedPortNotDefaultGateway_ExpectingTrueReturned();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::Router::PublishInterface
    * @tsd_testexpected expecting false returned
    */
   void test_PublishInterface_JustInvoke_ExpectingFalseReturned();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::Router::UnpublishInterface
    * @tsd_testexpected expecting no throws
    */
   void test_UnpublishInterface_JustInvoke_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke provided empty interface name
    *
    * @tsd_testobject tsd::communication::messaging::Router::LookupInterface
    * @tsd_testexpected expecting false returned
    */
   void test_LookupInterface_InvokeProvidedEmptyInterfaceName_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided local sender address
    *
    * @tsd_testobject tsd::communication::messaging::Router::JoinGroup
    * @tsd_testexpected expecting false returned
    */
   void test_JoinGroup_InvokeProvidedLocalSenderAddress_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided non local sender address existing in stubs
    *
    * @tsd_testobject tsd::communication::messaging::Router::JoinGroup
    * @tsd_testexpected expecting false returned
    */
   void test_JoinGroup_InvokeProvidedNonLocalSenderAddressExistingInStubs_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided non local sender address not in stubs
    *
    * @tsd_testobject tsd::communication::messaging::Router::JoinGroup
    * @tsd_testexpected expecting false returned
    */
   void test_JoinGroup_InvokeProvidedNonLocalSenderAddressNotInStubs_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided local sender and matching receiver
    *
    * @tsd_testobject tsd::communication::messaging::Router::LeaveGroup
    * @tsd_testexpected expecting true returned
    */
   void test_LeaveGroup_InvokeProvidedLocalSenderAndMatchingReceiver_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke provided local sender and not matching receiver
    *
    * @tsd_testobject tsd::communication::messaging::Router::LeaveGroup
    * @tsd_testexpected expecting false returned
    */
   void test_LeaveGroup_InvokeProvidedLocalSenderAndNotMatchingReceiver_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided non local sender and matching receiver
    *
    * @tsd_testobject tsd::communication::messaging::Router::LeaveGroup
    * @tsd_testexpected expecting true returned
    */
   void test_LeaveGroup_InvokeProvidedNonLocalSenderAndMatchingReceiver_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke provided non local sender and not matching receiver
    *
    * @tsd_testobject tsd::communication::messaging::Router::LeaveGroup
    * @tsd_testexpected expecting false returned
    */
   void test_LeaveGroup_InvokeProvidedNonLocalSenderAndNotMatchingReceiver_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided non local sender not in stubs and matching receiver
    *
    * @tsd_testobject tsd::communication::messaging::Router::LeaveGroup
    * @tsd_testexpected expecting false returned
    */
   void test_LeaveGroup_InvokeProvidedNonLocalSenderNotInStubsAndMatchingReceiver_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke when egress port null ingress not null packet type multicast join
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting true returned
    */
   void test_RoutePacket_InvokeWhenEgressPortNullIngressNotNullPacketTypeMulticastJoin_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when egress port null ingress port not null packet type multicast leave
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting true returned
    */
   void test_RoutePacket_InvokeWhenEgressPortNullIngressPortNotNullPacketTypeMulticastLeave_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when egress port null ingress port not null packet type death notification
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting true returned
    */
   void test_RoutePacket_InvokeWhenEgressPortNullIngressPortNotNullPacketTypeDeathNotification_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when egress port null ingress port null packet type unicast message
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting false returned
    */
   void test_RoutePacket_InvokeWhenEgressPortNullIngressPortNullPacketTypeUnicastMessage_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke when egress port null ingress port null packet type multicast message group not existing
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting false returned
    */
   void test_RoutePacket_InvokeWhenEgressPortNullIngressPortNullPacketTypeMulticastMessageGroupNotExisting_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke when packet type multicast message group existing found equal loop back address
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting false returned
    */
   void test_RoutePacket_InvokeWhenPacketTypeMulticastMessageGroupExistingFoundEqualLoopBackAddress_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke when egress port null ingress not null packet type dhcp offer
    *
    * @tsd_testobject tsd::communication::messaging::Router::RoutePacket
    * @tsd_testexpected expecting false returned
    */
   void test_RoutePacket_InvokeWhenEgressPortNullIngressNotNullPacketTypeDhcpOffer_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided sender receiver and message
    *
    * @tsd_testobject tsd::communication::messaging::Router::SendUnicastMessage
    * @tsd_testexpected expecting no throws
    */
   void test_SendUnicastMessage_InvokeProvidedSenderReceiverAndMessage_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when group existing and found is not local address
    *
    * @tsd_testobject tsd::communication::messaging::Router::SendBroadcastMessage
    * @tsd_testexpected expecting no throws
    */
   void test_SendBroadcastMessage_InvokeWhenGroupExistingAndFoundIsNotLocalAddress_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when group existing and found is local address
    *
    * @tsd_testobject tsd::communication::messaging::Router::SendBroadcastMessage
    * @tsd_testexpected expecting no throws
    */
   void test_SendBroadcastMessage_InvokeWhenGroupExistingAndFoundIsLocalAddress_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when group not existing
    *
    * @tsd_testobject tsd::communication::messaging::Router::SendBroadcastMessage
    * @tsd_testexpected expecting no throws
    */
   void test_SendBroadcastMessage_InvokeWhenGroupNotExisting_ExpectingNoThrows();
   /**
    * @brief Test scenario: just invoke
    *
    * @tsd_testobject tsd::communication::messaging::Router::GetName
    * @tsd_testexpected expecting name provided in constructor
    */
   void test_GetName_JustInvoke_ExpectingNameProvidedInConstructor();

   std::string                                        m_TestObjectName;
   std::shared_ptr<Router>                            m_TestObj;
   std::auto_ptr<Packet>                              m_TestPacket;
   std::auto_ptr<tsd::communication::event::TsdEvent> m_TestEvent;

   CPPUNIT_TEST_SUITE(RouterTest);
   CPPUNIT_TEST(test_Constructor_CreateWithName_ExpectingObjectCreatedAndMembersSet);
   CPPUNIT_TEST(test_GetLocalRouter_InvokeWhenLocalRouterNull_ExpectingRouterCreatedWithSpecificName);
   CPPUNIT_TEST(test_GetLocalRouter_InvokeWhenLocalRouterNotNull_ExpectingRouterReturnedWithSameAsFirstReference);
   CPPUNIT_TEST(test_AllocateIfcAddr_InvokeProvidedQueue_ExpectingSpecificAddressReturned);
   CPPUNIT_TEST(test_FreeIfcAddr_InvokeProvidedAddressNumber_ExpectingNoThrowsNotFurtherTestable);
   CPPUNIT_TEST(test_SetSubnetConfig_InvokeWhenSubNetsEmpty_ExpectingTrueReturned);
   CPPUNIT_TEST(test_SetSubnetConfig_InvokeWhenSubNetsNotEmpty_ExpectingFalseReturned);
   CPPUNIT_TEST(test_GetNameServer_JustRun_ExpectingServersAddressReturned);
   CPPUNIT_TEST(test_AllocateDownstreamAddr_InvokeWhenSubNetPrefixLengthIs0_ExpectingFalseReturned);
   CPPUNIT_TEST(test_AllocateDownstreamAddr_DefaultGatewayNull_ExpectingTrueReturned);
   CPPUNIT_TEST(test_AllocateDownstreamAddr_DefaultGatewayNotNull_ExpectingTrueReturned);
   CPPUNIT_TEST(test_FreeDownstreamAddr_InvokeAfterAddingAddress_ExpectingNoThrows);
   CPPUNIT_TEST(test_AddDownstreamPort_InvokeProvidedPort_ExpectingNoThrows);
   CPPUNIT_TEST(test_AddUpstreamPort_InvokeWhenDefaultGatewayNullSubNetsNotEmptySubDomainEmpty_ExpectingTrueReturned);
   CPPUNIT_TEST(test_AddUpstreamPort_InvokeWhenDefaultGatewayNullSubNetsNotEmptySubDomainNotEmpty_ExpectingTrueReturned);
   CPPUNIT_TEST(test_AddUpstreamPort_InvokeWhenDefaultGatewayNotNull_ExpectingFalseReturned);
   CPPUNIT_TEST(test_AddUpstreamPort_InvokeWhenSubNetsNotEmpty_ExpectingFalseReturned);
   CPPUNIT_TEST(test_DelPort_InvokeProvidedPortExistingInPortsAndIsDefaultGateway_ExpectingNoThrows);
   CPPUNIT_TEST(test_DelPort_InvokeProvidedPortNotExistingInPortsAndNotDefaultGateway_ExpectingNoThrows);
   CPPUNIT_TEST(test_IsAnyPortAddr_InvokeProvidedPortSameToDefaultGateway_ExpectingTrueReturned);
   CPPUNIT_TEST(test_IsAnyPortAddr_InvokeProvidedPortExistingInPortsDefaultGatewayNull_ExpectingTrueReturned);
   CPPUNIT_TEST(test_IsAnyPortAddr_InvokeProvidedPortNotDefaultGateway_ExpectingTrueReturned);
   CPPUNIT_TEST(test_PublishInterface_JustInvoke_ExpectingFalseReturned);
   CPPUNIT_TEST(test_UnpublishInterface_JustInvoke_ExpectingNoThrows);
   CPPUNIT_TEST(test_LookupInterface_InvokeProvidedEmptyInterfaceName_ExpectingFalseReturned);
   CPPUNIT_TEST(test_JoinGroup_InvokeProvidedLocalSenderAddress_ExpectingFalseReturned);
   CPPUNIT_TEST(test_JoinGroup_InvokeProvidedNonLocalSenderAddressExistingInStubs_ExpectingFalseReturned);
   CPPUNIT_TEST(test_JoinGroup_InvokeProvidedNonLocalSenderAddressNotInStubs_ExpectingFalseReturned);
   CPPUNIT_TEST(test_LeaveGroup_InvokeProvidedLocalSenderAndMatchingReceiver_ExpectingTrueReturned);
   CPPUNIT_TEST(test_LeaveGroup_InvokeProvidedLocalSenderAndNotMatchingReceiver_ExpectingFalseReturned);
   CPPUNIT_TEST(test_LeaveGroup_InvokeProvidedNonLocalSenderAndMatchingReceiver_ExpectingTrueReturned);
   CPPUNIT_TEST(test_LeaveGroup_InvokeProvidedNonLocalSenderAndNotMatchingReceiver_ExpectingFalseReturned);
   CPPUNIT_TEST(test_LeaveGroup_InvokeProvidedNonLocalSenderNotInStubsAndMatchingReceiver_ExpectingFalseReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenEgressPortNullIngressNotNullPacketTypeMulticastJoin_ExpectingTrueReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenEgressPortNullIngressPortNotNullPacketTypeMulticastLeave_ExpectingTrueReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenEgressPortNullIngressPortNotNullPacketTypeDeathNotification_ExpectingTrueReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenEgressPortNullIngressPortNullPacketTypeUnicastMessage_ExpectingFalseReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenEgressPortNullIngressPortNullPacketTypeMulticastMessageGroupNotExisting_ExpectingFalseReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenPacketTypeMulticastMessageGroupExistingFoundEqualLoopBackAddress_ExpectingFalseReturned);
   CPPUNIT_TEST(test_RoutePacket_InvokeWhenEgressPortNullIngressNotNullPacketTypeDhcpOffer_ExpectingFalseReturned);
   CPPUNIT_TEST(test_SendUnicastMessage_InvokeProvidedSenderReceiverAndMessage_ExpectingNoThrows);
   CPPUNIT_TEST(test_SendBroadcastMessage_InvokeWhenGroupExistingAndFoundIsNotLocalAddress_ExpectingNoThrows);
   CPPUNIT_TEST(test_SendBroadcastMessage_InvokeWhenGroupExistingAndFoundIsLocalAddress_ExpectingNoThrows);
   CPPUNIT_TEST(test_SendBroadcastMessage_InvokeWhenGroupNotExisting_ExpectingNoThrows);
   CPPUNIT_TEST(test_GetName_JustInvoke_ExpectingNameProvidedInConstructor);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_ROUTERTEST_HPP
