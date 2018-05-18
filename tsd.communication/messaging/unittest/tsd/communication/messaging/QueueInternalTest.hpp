//////////////////////////////////////////////////////////////////////
/// @file QueueInternalTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test QueueInternal
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_QUEUEINTERNALTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_QUEUEINTERNALTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace event {

class TsdEvent;

} // namespace event

namespace messaging {

class Queue;
class Router;
class NameServerProtocol;

/**
 * Testclass for Queue
 *
 * @brief Testclass for Queue
 */
class QueueTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Destructor to clear singletons.
    */
   virtual ~QueueTest();

   void pushMessageAfterDelay(uint32_t delay);
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief Test scenario: just create provided name and router
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Ctor
    * @tsd_testexpected expecting object created and members set
    */
   void test_Ctor_JustCreateProvidedNameAndRouter_ExpectingObjectCreatedAndMembersSet();
   /**
    * @brief Test scenario: invoke when interface name not empty and publish interface false
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::RegisterInterface
    * @tsd_testexpected expecting exception thrown
    */
   void test_RegisterInterface_InvokeWhenInterfaceNameNotEmptyAndPublishInterfaceFalse_ExpectingExceptionThrown();
   /**
    * @brief Test scenario: invoke when interface name not empty and publish interface true
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::RegisterInterface
    * @tsd_testexpected expecting valid pointer returned
    */
   void test_RegisterInterface_InvokeWhenInterfaceNameNotEmptyAndPublishInterfaceTrue_ExpectingValidPointerReturned();
   /**
    * @brief Test scenario: invoke when interface name empty
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::RegisterInterface
    * @tsd_testexpected expecting valid pointer returned
    */
   void test_RegisterInterface_InvokeWhenInterfaceNameEmpty_ExpectingValidPointerReturned();
   /**
    * @brief Test scenario: invoke when join group returns true
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting no throws
    */
   void test_ConnectInterface_InvokeWhenJoinGroupReturnsTrue_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when join group returns false
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting no throws
    */
   void test_ConnectInterface_InvokeWhenJoinGroupReturnsFalse_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when lookup interface returns true join group returns true
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting no throws and valid ptr returned
    */
   void test_ConnectInterface_InvokeWhenLookupInterfaceReturnsTrueJoinGroupReturnsTrue_ExpectingNoThrowsAndValidPtrReturned();
   /**
    * @brief Test scenario: invoke when lookup interface returns true join group returns false
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting no throws and valid ptr returned
    */
   void test_ConnectInterface_InvokeWhenLookupInterfaceReturnsTrueJoinGroupReturnsFalse_ExpectingNoThrowsAndValidPtrReturned();
   /**
    * @brief Test scenario: invoke when lookup interface returns false
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting null returned
    */
   void test_ConnectInterface_InvokeWhenLookupInterfaceReturnsFalse_ExpectingNullReturned();
   /**
    * @brief Test scenario: invoke when local ifc null
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting throw
    */
   void test_ConnectInterface_InvokeWhenLocalIfcNull_ExpectingThrow();
   /**
    * @brief Test scenario: invoke when local ifc with same queue
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting throw
    */
   void test_ConnectInterface_InvokeWhenLocalIfcWithSameQueue_ExpectingThrow();
   /**
    * @brief Test scenario: invoke when local ifc with differet queue and different router
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting throw
    */
   void test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndDifferentRouter_ExpectingThrow();
   /**
    * @brief Test scenario: invoke when local ifc with differet queue and same router join group true
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting valid ptr returned
    */
   void test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndSameRouterJoinGroupTrue_ExpectingValidPtrReturned();
   /**
    * @brief Test scenario: invoke when local ifc with differet queue and same router join group false
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ConnectInterface
    * @tsd_testexpected expecting valid ptr returned
    */
   void test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndSameRouterJoinGroupFalse_ExpectingValidPtrReturned();
   /**
    * @brief Test scenario: invoke when timers empty timeout not infinite pull message not null
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ReadMessage
    * @tsd_testexpected expecting message returned previously pushed
    */
   void test_ReadMessage_InvokeWhenTimersEmptyTimeoutNotInfinitePullMessageNotNull_ExpectingMessageReturnedPreviouslyPushed();
   /**
    * @brief Test scenario: invoke when timers empty timeout infinite pull message not null
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ReadMessage
    * @tsd_testexpected expecting message returned previously pushed
    */
   void test_ReadMessage_InvokeWhenTimersEmptyTimeoutInfinitePullMessageNotNull_ExpectingMessageReturnedPreviouslyPushed();
   /**
    * @brief Test scenario: invoke when timers not empty timeout not infinite pull message null
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ReadMessage
    * @tsd_testexpected expecting null message returned
    */
   void test_ReadMessage_InvokeWhenTimersNotEmptyTimeoutNotInfinitePullMessageNull_ExpectingNullMessageReturned();
   /**
    * @brief Test scenario: invoke when timers empty timeout infinite pull message not null after delay
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::ReadMessage
    * @tsd_testexpected expecting message returned previously pushed
    */
   void test_ReadMessage_InvokeWhenTimersEmptyTimeoutInfinitePullMessageNotNullAfterDelay_ExpectingMessageReturnedPreviouslyPushed();
   /**
    * @brief Test scenario: invoke provided message
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::SendSelfMessage
    * @tsd_testexpected expecting no throws
    */
   void test_SendSelfMessage_InvokeProvidedMessage_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke after already adding timer
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::StartTimer
    * @tsd_testexpected expecting unique refs returned
    */
   void test_StartTimer_InvokeAfterAlreadyAddingTimer_ExpectingUniqueRefsReturned();
   /**
    * @brief Test scenario: invoke when adding existing ref
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::StartTimer
    * @tsd_testexpected case not testable
    */
   void test_StartTimer_InvokeWhenAddingExistingRef_CaseNotTestable();
   /**
    * @brief Test scenario: invoke provided existing ref in timers cyclic
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::StopTimer
    * @tsd_testexpected expecting true returned
    */
   void test_StopTimer_InvokeProvidedExistingRefInTimersCyclic_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke provided existing ref in timers one time
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::StopTimer
    * @tsd_testexpected expecting true returned
    */
   void test_StopTimer_InvokeProvidedExistingRefInTimersOneTime_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke provided not existing ref in timers
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::StopTimer
    * @tsd_testexpected expecting true returned
    */
   void test_StopTimer_InvokeProvidedNotExistingRefInTimers_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke after creating with name
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::GetName
    * @tsd_testexpected expecting name matching with one passed to ctor
    */
   void test_GetName_InvokeAfterCreatingWithName_ExpectingNameMatchingWithOnePassedToCtor();
   /**
    * @brief Test scenario: invoke provided all arguments
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::InterfaceAdded
    * @tsd_testexpected expecting no throws
    */
   void test_InterfaceAdded_InvokeProvidedAllArguments_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when event with same receiver address exists in event queue
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::InterfaceRemoved
    * @tsd_testexpected expecting no throws
    */
   void test_InterfaceRemoved_InvokeWhenEventWithSameReceiverAddressExistsInEventQueue_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when event address mismatches one in event queue
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::InterfaceRemoved
    * @tsd_testexpected expecting no throws
    */
   void test_InterfaceRemoved_InvokeWhenEventAddressMismatchesOneInEventQueue_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when timers not empty multicast true notification not null
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushMessage
    * @tsd_testexpected expecting no throws
    */
   void test_PushMessage_InvokeWhenTimersNotEmptyMulticastTrueNotificationNotNull_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when timers empty multicast true notification null
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushMessage
    * @tsd_testexpected expecting no throws
    */
   void test_PushMessage_InvokeWhenTimersEmptyMulticastTrueNotificationNull_ExpectingNoThrows();
   /**
    * @brief Test scenario: with local ifc and multicast true
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushMessage
    * @tsd_testexpected message not put into queue
    */
   void test_PushMessage_WithLocalIfcAndMulticastTrue_MessageNotPutIntoQueue();
   /**
    * @brief Test scenario: invoke when timers empty multicast false
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushMessage
    * @tsd_testexpected expecting no throws
    */
   void test_PushMessage_InvokeWhenTimersEmptyMulticastFalse_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke when factory exists and provides valid message
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushPacket
    * @tsd_testexpected expecting true returned
    */
   void test_PushPacket_InvokeWhenFactoryExistsAndProvidesValidMessage_ExpectingTrueReturned();
   /**
    * @brief Test scenario: invoke when factory exists and provides null message
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushPacket
    * @tsd_testexpected expecting false returned
    */
   void test_PushPacket_InvokeWhenFactoryExistsAndProvidesNullMessage_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke when factory doesnt exist
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PushPacket
    * @tsd_testexpected expecting false returned
    */
   void test_PushPacket_InvokeWhenFactoryDoesntExist_ExpectingFalseReturned();
   /**
    * @brief Test scenario: invoke provided non zero ref and2 existing messages
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PurgeMessages
    * @tsd_testexpected expecting no throws
    */
   void test_PurgeMessages_InvokeProvidedNonZeroRefAnd2ExistingMessages_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke provided0 ref
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::PurgeMessages
    * @tsd_testexpected expecting no throws
    */
   void test_PurgeMessages_InvokeProvided0Ref_ExpectingNoThrows();
   /**
    * @brief Test scenario: invoke after creating with router
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::GetRouter
    * @tsd_testexpected expecting matching references of routers
    */
   void test_GetRouter_InvokeAfterCreatingWithRouter_ExpectingMatchingReferencesOfRouters();
   /**
    * @brief Test scenario: invoke twice then compare
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::MakeRef
    * @tsd_testexpected expecting unique references returned
    */
   void test_MakeRef_InvokeTwiceThenCompare_ExpectingUniqueReferencesReturned();
   /**
    * @brief Test scenario: invoke when receiver address matches one in notifications
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Dead
    * @tsd_testexpected expect call on notifier
    */
   void test_Dead_InvokeWhenReceiverAddressMatchesOneInNotifications_ExpectCallOnNotifier();
   /**
    * @brief Test scenario: invoke with real objects
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Dead
    * @tsd_testexpected notify dead expcted
    */
   void test_Dead_InvokeWithRealObjects_NotifyDeadExpcted();
   /**
    * @brief Test scenario: invoke when receiver address doesnt match one in notifications
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Dead
    * @tsd_testexpected expect no calls on notifier
    */
   void test_Dead_InvokeWhenReceiverAddressDoesntMatchOneInNotifications_ExpectNoCallsOnNotifier();

   std::shared_ptr<Queue>                             m_TestObject;
   std::shared_ptr<Router>                            m_TestRouter;
   std::auto_ptr<tsd::communication::event::TsdEvent> m_TestMessage;

   CPPUNIT_TEST_SUITE(QueueTest);
   CPPUNIT_TEST(test_Ctor_JustCreateProvidedNameAndRouter_ExpectingObjectCreatedAndMembersSet);
   CPPUNIT_TEST(test_RegisterInterface_InvokeWhenInterfaceNameNotEmptyAndPublishInterfaceFalse_ExpectingExceptionThrown);
   CPPUNIT_TEST(test_RegisterInterface_InvokeWhenInterfaceNameNotEmptyAndPublishInterfaceTrue_ExpectingValidPointerReturned);
   CPPUNIT_TEST(test_RegisterInterface_InvokeWhenInterfaceNameEmpty_ExpectingValidPointerReturned);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenJoinGroupReturnsTrue_ExpectingNoThrows);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenJoinGroupReturnsFalse_ExpectingNoThrows);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLookupInterfaceReturnsTrueJoinGroupReturnsTrue_ExpectingNoThrowsAndValidPtrReturned);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLookupInterfaceReturnsTrueJoinGroupReturnsFalse_ExpectingNoThrowsAndValidPtrReturned);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLookupInterfaceReturnsFalse_ExpectingNullReturned);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLocalIfcNull_ExpectingThrow);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLocalIfcWithSameQueue_ExpectingThrow);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndDifferentRouter_ExpectingThrow);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndSameRouterJoinGroupTrue_ExpectingValidPtrReturned);
   CPPUNIT_TEST(test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndSameRouterJoinGroupFalse_ExpectingValidPtrReturned);
   CPPUNIT_TEST(test_ReadMessage_InvokeWhenTimersEmptyTimeoutNotInfinitePullMessageNotNull_ExpectingMessageReturnedPreviouslyPushed);
   CPPUNIT_TEST(test_ReadMessage_InvokeWhenTimersEmptyTimeoutInfinitePullMessageNotNull_ExpectingMessageReturnedPreviouslyPushed);
   CPPUNIT_TEST(test_ReadMessage_InvokeWhenTimersNotEmptyTimeoutNotInfinitePullMessageNull_ExpectingNullMessageReturned);
   CPPUNIT_TEST(test_ReadMessage_InvokeWhenTimersEmptyTimeoutInfinitePullMessageNotNullAfterDelay_ExpectingMessageReturnedPreviouslyPushed);
   CPPUNIT_TEST(test_SendSelfMessage_InvokeProvidedMessage_ExpectingNoThrows);
   CPPUNIT_TEST(test_StartTimer_InvokeAfterAlreadyAddingTimer_ExpectingUniqueRefsReturned);
   CPPUNIT_TEST(test_StartTimer_InvokeWhenAddingExistingRef_CaseNotTestable);
   CPPUNIT_TEST(test_StopTimer_InvokeProvidedExistingRefInTimersCyclic_ExpectingTrueReturned);
   CPPUNIT_TEST(test_StopTimer_InvokeProvidedExistingRefInTimersOneTime_ExpectingTrueReturned);
   CPPUNIT_TEST(test_StopTimer_InvokeProvidedNotExistingRefInTimers_ExpectingTrueReturned);
   CPPUNIT_TEST(test_GetName_InvokeAfterCreatingWithName_ExpectingNameMatchingWithOnePassedToCtor);
   CPPUNIT_TEST(test_InterfaceAdded_InvokeProvidedAllArguments_ExpectingNoThrows);
   CPPUNIT_TEST(test_InterfaceRemoved_InvokeWhenEventWithSameReceiverAddressExistsInEventQueue_ExpectingNoThrows);
   CPPUNIT_TEST(test_InterfaceRemoved_InvokeWhenEventAddressMismatchesOneInEventQueue_ExpectingNoThrows);
   CPPUNIT_TEST(test_PushMessage_InvokeWhenTimersNotEmptyMulticastTrueNotificationNotNull_ExpectingNoThrows);
   CPPUNIT_TEST(test_PushMessage_InvokeWhenTimersEmptyMulticastTrueNotificationNull_ExpectingNoThrows);
   CPPUNIT_TEST(test_PushMessage_WithLocalIfcAndMulticastTrue_MessageNotPutIntoQueue);
   CPPUNIT_TEST(test_PushMessage_InvokeWhenTimersEmptyMulticastFalse_ExpectingNoThrows);
   CPPUNIT_TEST(test_PushPacket_InvokeWhenFactoryExistsAndProvidesValidMessage_ExpectingTrueReturned);
   CPPUNIT_TEST(test_PushPacket_InvokeWhenFactoryExistsAndProvidesNullMessage_ExpectingFalseReturned);
   CPPUNIT_TEST(test_PushPacket_InvokeWhenFactoryDoesntExist_ExpectingFalseReturned);
   CPPUNIT_TEST(test_PurgeMessages_InvokeProvidedNonZeroRefAnd2ExistingMessages_ExpectingNoThrows);
   CPPUNIT_TEST(test_PurgeMessages_InvokeProvided0Ref_ExpectingNoThrows);
   CPPUNIT_TEST(test_GetRouter_InvokeAfterCreatingWithRouter_ExpectingMatchingReferencesOfRouters);
   CPPUNIT_TEST(test_MakeRef_InvokeTwiceThenCompare_ExpectingUniqueReferencesReturned);
   CPPUNIT_TEST(test_Dead_InvokeWhenReceiverAddressMatchesOneInNotifications_ExpectCallOnNotifier);
   CPPUNIT_TEST(test_Dead_InvokeWithRealObjects_NotifyDeadExpcted);
   CPPUNIT_TEST(test_Dead_InvokeWhenReceiverAddressDoesntMatchOneInNotifications_ExpectNoCallsOnNotifier);
   CPPUNIT_TEST_SUITE_END();
};

class IRemoteIfc;
class RemoteIfcMembers;

/**
 * Testclass for RemoteIfc
 *
 * @brief Testclass for RemoteIfc
 */
class RemoteIfcTest : public CPPUNIT_NS::TestFixture
{
public:
   std::shared_ptr<RemoteIfcMembers> m_Members;
   std::shared_ptr<IRemoteIfc>       m_TestObj;
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;

   void verifySubscribed(uint32_t id);
   void verifyUnsubscribed(uint32_t id);
   /**
    * @brief Test scenario: with vector
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Subscribe
    * @tsd_testexpected subscribed
    */
   void test_Subscribe_WithVector_Subscribed();
   /**
    * @brief Test scenario: with id
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Unsubscribe
    * @tsd_testexpected subscribed
    */
   void test_Unsubscribe_WithId_Subscribed();
   /**
    * @brief Test scenario: with vector
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Unsubscribe
    * @tsd_testexpected subscribed
    */
   void test_Unsubscribe_WithVector_Subscribed();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::QueueInternal::Demonitor
    * @tsd_testexpected events destroyed
    */
   void test_Demonitor_JustRun_EventsDestroyed();

   CPPUNIT_TEST_SUITE(RemoteIfcTest);
   CPPUNIT_TEST(test_Subscribe_WithVector_Subscribed);
   CPPUNIT_TEST(test_Unsubscribe_WithId_Subscribed);
   CPPUNIT_TEST(test_Unsubscribe_WithVector_Subscribed);
   CPPUNIT_TEST(test_Demonitor_JustRun_EventsDestroyed);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_QUEUEINTERNALTEST_HPP
