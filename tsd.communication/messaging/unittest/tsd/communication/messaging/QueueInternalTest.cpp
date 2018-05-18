//////////////////////////////////////////////////////////////////////
/// @file QueueInternalTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test QueueInternal
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "QueueInternalTest.hpp"
#include <chrono>
#include <functional>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/AddressInUseException.hpp>
#include <tsd/communication/messaging/IIfcNotifiyMock.hpp>
#include <tsd/communication/messaging/IPortMock.hpp>
#include <tsd/communication/messaging/IQueueMock.hpp>
#include <tsd/communication/messaging/InvalidArgumentException.hpp>
#include <tsd/communication/messaging/NameServer.cpp>
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>

namespace tsd {
namespace communication {
namespace messaging {

using testing::_;
using testing::Mock;
using testing::Return;

class RunnableImpl : public tsd::common::system::Runnable
{
public:
   RunnableImpl(std::function<void(std::shared_ptr<Queue>, tsd::communication::event::TsdEvent*)> runnable,
                std::shared_ptr<Queue>               q,
                tsd::communication::event::TsdEvent* e)
   {
      m_Fnc     = runnable;
      m_Queue   = q;
      m_Message = e;
   }
   void run()
   {
      m_Fnc(m_Queue, m_Message);
   }

private:
   std::function<void(std::shared_ptr<Queue>, tsd::communication::event::TsdEvent*)> m_Fnc;
   std::shared_ptr<Queue>               m_Queue;
   tsd::communication::event::TsdEvent* m_Message;
};

QueueTest::~QueueTest()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void QueueTest::setUp()
{
   std::string testRouterName{"testRouter"}, testQueueName{"testQueueName"};
   m_TestRouter = std::make_shared<Router>(testRouterName);
   m_TestObject = std::make_shared<Queue>(testQueueName, *m_TestRouter.get());
}

void QueueTest::test_Ctor_JustCreateProvidedNameAndRouter_ExpectingObjectCreatedAndMembersSet()
{
   std::string            testName("testQueue");
   Router                 testRouter("testRouter");
   std::unique_ptr<Queue> testObj(new Queue(testName, testRouter));
   CPPUNIT_ASSERT_MESSAGE("Queue null after creating with constructor", testObj != nullptr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Name from getName doesn't match one provided in ctor", testName, testObj->getName());
}

void QueueTest::test_RegisterInterface_InvokeWhenInterfaceNameNotEmptyAndPublishInterfaceFalse_ExpectingExceptionThrown()
{
   IMessageFactoryMock testMsgFc;
   std::string         testInterfaceName("testInterfaceName");

   tsd::communication::event::IfcAddr_t addr = m_TestRouter->allocateIfcAddr(m_TestObject.get());
   m_TestRouter->publishInterface(testInterfaceName, addr);
   CPPUNIT_ASSERT_THROW_MESSAGE("Exception not thrown when expected to",
                                (std::shared_ptr<ILocalIfc>(m_TestObject->registerInterface(testMsgFc, testInterfaceName))),
                                tsd::communication::messaging::AddressInUseException);
}

void QueueTest::test_RegisterInterface_InvokeWhenInterfaceNameNotEmptyAndPublishInterfaceTrue_ExpectingValidPointerReturned()
{
   std::string                testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc> retIfc;
   IMessageFactoryMock        testMsgFc;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception thrown when unexpected",
                                   (retIfc.reset(m_TestObject->registerInterface(testMsgFc, testInterfaceName))));
   CPPUNIT_ASSERT_MESSAGE("Interface returned by registerInterface null", retIfc != nullptr);
   // to increase coverage - the only verification here is Valgrind report
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1));
   retIfc->monitor(m_TestMessage, retIfc->getLocalIfcAddr());
}

void QueueTest::test_RegisterInterface_InvokeWhenInterfaceNameEmpty_ExpectingValidPointerReturned()
{
   std::string                testInterfaceName("");
   std::shared_ptr<ILocalIfc> retIfc;
   IMessageFactoryMock        testMsgFc;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Exception thrown when unexpected",
                                   (retIfc.reset(m_TestObject->registerInterface(testMsgFc, testInterfaceName))));
   CPPUNIT_ASSERT_MESSAGE("Interface returned by registerInterface null", retIfc != nullptr);
}

void QueueTest::test_ConnectInterface_InvokeWhenJoinGroupReturnsTrue_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0), localAddr(0);
   IMessageFactoryMock                  testMsgFc;
   std::shared_ptr<IRemoteIfc>          retIfc;
   m_TestRouter->joinGroup(testAddr, localAddr);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("connectInterface failed with a throw",
                                   (retIfc.reset(m_TestObject->connectInterface(testAddr, testMsgFc))));
   CPPUNIT_ASSERT_MESSAGE("Interface returned is unexpectedly null", retIfc != nullptr);
}

void QueueTest::test_ConnectInterface_InvokeWhenJoinGroupReturnsFalse_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0);
   IMessageFactoryMock                  testMsgFc;
   std::shared_ptr<IRemoteIfc>          retIfc;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("connectInterface failed with a throw",
                                   (retIfc.reset(m_TestObject->connectInterface(testAddr, testMsgFc))));
   CPPUNIT_ASSERT_MESSAGE("Interface returned is unexpectedly null", retIfc != nullptr);
   // verify notify dead:
   std::auto_ptr<event::TsdEvent> ev(new event::TsdEvent(0));
   IMessageSelectorMock           selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(ev.get())).WillOnce(testing::Return(true));
   retIfc->monitor(ev);
   m_TestObject->readMessage(1, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectatios shall be met", testing::Mock::VerifyAndClearExpectations(&selector));
}

void QueueTest::test_ConnectInterface_InvokeWhenLookupInterfaceReturnsTrueJoinGroupReturnsTrue_ExpectingNoThrowsAndValidPtrReturned()
{
   std::string         testInterfaceName("testInterface");
   IMessageFactoryMock testMsgFc;
   uint32_t            testTimeout(1000);
   // NameServerProtocol test;
   m_TestMessage.reset(new tsd::communication::messaging::QueryNameReply(true, false, tsd::communication::event::IfcAddr_t(0)));
   m_TestObject->pushMessage(m_TestMessage, 0);
   std::shared_ptr<ILocalIfc>  localIfc(m_TestObject->registerInterface(testMsgFc, testInterfaceName));
   std::shared_ptr<IRemoteIfc> retInterface;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("ConnectInterface failed with a throw",
                                   (retInterface.reset(m_TestObject->connectInterface(testInterfaceName, testMsgFc, testTimeout))));
   CPPUNIT_ASSERT_MESSAGE("Interface returned from connectInterface null", retInterface != nullptr);
}

void QueueTest::test_ConnectInterface_InvokeWhenLookupInterfaceReturnsTrueJoinGroupReturnsFalse_ExpectingNoThrowsAndValidPtrReturned()
{
   std::string         testInterfaceName("testInterface");
   IMessageFactoryMock testMsgFc;
   uint32_t            testTimeout(1000);
   // NameServerProtocol test;
   m_TestMessage.reset(new tsd::communication::messaging::QueryNameReply(true, false, tsd::communication::event::IfcAddr_t(0)));
   m_TestObject->pushMessage(m_TestMessage, 0);
   std::shared_ptr<ILocalIfc>  localIfc(m_TestObject->registerInterface(testMsgFc, testInterfaceName));
   std::shared_ptr<IRemoteIfc> retInterface;
   m_TestObject->getRouter().freeIfcAddr(localIfc->getLocalIfcAddr());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("ConnectInterface failed with a throw",
                                   retInterface.reset(m_TestObject->connectInterface(testInterfaceName, testMsgFc, testTimeout)));
   CPPUNIT_ASSERT_MESSAGE("Interface returned from connectInterface null", retInterface != nullptr);
   // verify notify dead:
   std::auto_ptr<event::TsdEvent> ev(new event::TsdEvent(0));
   IMessageSelectorMock           selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(ev.get())).WillOnce(testing::Return(true));
   retInterface->monitor(ev);
   m_TestObject->readMessage(1, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectatios shall be met", testing::Mock::VerifyAndClearExpectations(&selector));
}

void QueueTest::test_ConnectInterface_InvokeWhenLookupInterfaceReturnsFalse_ExpectingNullReturned()
{
   std::string                 testInterfaceName("testInterface");
   IMessageFactoryMock         testMsgFc;
   uint32_t                    testTimeout(1000);
   std::shared_ptr<IRemoteIfc> retIfc;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("ConnectInterface failed with a throw",
                                   (retIfc.reset(m_TestObject->connectInterface(testInterfaceName, testMsgFc, testTimeout))));
   CPPUNIT_ASSERT_MESSAGE("Interface returned valid ptr from connectInterface unexpectedly", retIfc == nullptr);
}

void QueueTest::test_ConnectInterface_InvokeWhenLocalIfcNull_ExpectingThrow()
{
   ILocalIfc*          testLocalIfc = nullptr;
   IMessageFactoryMock testMsgFc;
   CPPUNIT_ASSERT_THROW_MESSAGE("connectInterface didnt fail with a throw unexpectedly",
                                m_TestObject->connectInterface(testLocalIfc, testMsgFc),
                                tsd::communication::messaging::InvalidArgumentException);
}

void QueueTest::test_ConnectInterface_InvokeWhenLocalIfcWithSameQueue_ExpectingThrow()
{
   IMessageFactoryMock        testMsgFc;
   std::string                testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc> testLocalIfc(m_TestObject->registerInterface(testMsgFc, testInterfaceName));
   CPPUNIT_ASSERT_THROW_MESSAGE("connectInterface didnt fail with a throw unexpectedly",
                                m_TestObject->connectInterface(testLocalIfc.get(), testMsgFc),
                                tsd::communication::messaging::InvalidArgumentException);
}

void QueueTest::test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndDifferentRouter_ExpectingThrow()
{
   Router                     testTempRouter("tempRouter");
   Queue                      testTempQueue("tempQueue", testTempRouter);
   IMessageFactoryMock        testMsgFc;
   std::string                testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc> testLocalIfc(testTempQueue.registerInterface(testMsgFc, testInterfaceName));
   CPPUNIT_ASSERT_THROW_MESSAGE("connectInterface didnt fail with a throw unexpectedly",
                                m_TestObject->connectInterface(testLocalIfc.get(), testMsgFc),
                                tsd::communication::messaging::InvalidArgumentException);
}

void QueueTest::test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndSameRouterJoinGroupTrue_ExpectingValidPtrReturned()
{
   Queue                                testTempQueue("tempQueue", *m_TestRouter.get());
   IMessageFactoryMock                  testMsgFc;
   std::string                          testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc>           testLocalIfc(testTempQueue.registerInterface(testMsgFc, testInterfaceName));
   tsd::communication::event::IfcAddr_t testAddr(0), localAddr(0);
   m_TestRouter->joinGroup(testAddr, localAddr);
   std::shared_ptr<IRemoteIfc> retInterface;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("connectInterface failed with a throw",
                                   (retInterface.reset(m_TestObject->connectInterface(testLocalIfc.get(), testMsgFc))));
   CPPUNIT_ASSERT_MESSAGE("connectInterface returned nullptr unexpectedly", retInterface != nullptr);
}

void QueueTest::test_ConnectInterface_InvokeWhenLocalIfcWithDifferetQueueAndSameRouterJoinGroupFalse_ExpectingValidPtrReturned()
{
   Queue                      testTempQueue("tempQueue", *m_TestRouter.get());
   IMessageFactoryMock        testMsgFc;
   std::string                testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc> testLocalIfc(testTempQueue.registerInterface(testMsgFc, testInterfaceName));
   m_TestRouter->freeIfcAddr(testLocalIfc->getLocalIfcAddr());
   std::shared_ptr<IRemoteIfc> retInterface;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("connectInterface failed with a throw",
                                   (retInterface.reset(m_TestObject->connectInterface(testLocalIfc.get(), testMsgFc))));
   CPPUNIT_ASSERT_MESSAGE("connectInterface returned nullptr unexpectedly", retInterface != nullptr);
   // verify notify dead:
   std::auto_ptr<event::TsdEvent> ev(new event::TsdEvent(0));
   IMessageSelectorMock           selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(ev.get())).WillOnce(testing::Return(true));
   retInterface->monitor(ev);
   m_TestObject->readMessage(1, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectatios shall be met", testing::Mock::VerifyAndClearExpectations(&selector));
}

void QueueTest::test_ReadMessage_InvokeWhenTimersEmptyTimeoutNotInfinitePullMessageNotNull_ExpectingMessageReturnedPreviouslyPushed()
{
   uint32_t testTimeout(1000);
   uint32_t testEventId(1u);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(testEventId));
   m_TestObject->pushMessage(m_TestMessage, 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic                                ignored "-Wdeprecated-declarations"
   std::auto_ptr<tsd::communication::event::TsdEvent> retMsg;
#pragma GCC diagnostic pop
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("readMessage failed with a throw", (retMsg = m_TestObject->readMessage(testTimeout)));
   CPPUNIT_ASSERT_MESSAGE("Message returned from readMessage null", retMsg.get() != nullptr);
   CPPUNIT_ASSERT_MESSAGE("Message not equal to one pushed", retMsg->getEventId() == testEventId);
}

void QueueTest::test_ReadMessage_InvokeWhenTimersEmptyTimeoutInfinitePullMessageNotNull_ExpectingMessageReturnedPreviouslyPushed()
{
   uint32_t testEventId(1u);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(testEventId));
   m_TestObject->pushMessage(m_TestMessage, 0);
#pragma GCC diagnostic push
#pragma GCC diagnostic                                ignored "-Wdeprecated-declarations"
   std::auto_ptr<tsd::communication::event::TsdEvent> retMsg;
#pragma GCC diagnostic pop
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("readMessage failed with a throw", (retMsg = m_TestObject->readMessage()));
   CPPUNIT_ASSERT_MESSAGE("Message returned from readMessage null", retMsg.get() != nullptr);
   CPPUNIT_ASSERT_MESSAGE("Message not equal to one pushed", retMsg->getEventId() == testEventId);
}

void QueueTest::test_ReadMessage_InvokeWhenTimersNotEmptyTimeoutNotInfinitePullMessageNull_ExpectingNullMessageReturned()
{
   uint32_t testTimeout(1);
#pragma GCC diagnostic push
#pragma GCC diagnostic                                ignored "-Wdeprecated-declarations"
   std::auto_ptr<tsd::communication::event::TsdEvent> retMsg;
   std::auto_ptr<tsd::communication::event::TsdEvent> timerMsg(new event::TsdEvent(0));
#pragma GCC diagnostic pop
   m_TestObject->startTimer(timerMsg, 1000000, true);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("readMessage failed with a throw", (retMsg = m_TestObject->readMessage(testTimeout)));
   CPPUNIT_ASSERT_MESSAGE("Message returned from readMessage not null unexpectedly", retMsg.get() == nullptr);
}

void QueueTest::test_ReadMessage_InvokeWhenTimersEmptyTimeoutInfinitePullMessageNotNullAfterDelay_ExpectingMessageReturnedPreviouslyPushed()
{
   uint32_t testEventId(1u);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   std::function<void(std::shared_ptr<Queue>, tsd::communication::event::TsdEvent*)> func(
      [](std::shared_ptr<Queue> tobj, tsd::communication::event::TsdEvent* tmsg) {
         tsd::common::system::Thread::sleep(1000);
         tobj->startTimer(std::auto_ptr<tsd::communication::event::TsdEvent>(tmsg), 1000, true);
      });
   RunnableImpl                runFunc(func, m_TestObject, m_TestMessage.release());
   tsd::common::system::Thread diffThread("TestThread1", &runFunc);
   diffThread.start();
   std::auto_ptr<tsd::communication::event::TsdEvent> retMsg;
#pragma GCC diagnostic pop
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("readMessage failed with a throw", (retMsg = m_TestObject->readMessage()));
   diffThread.join();
   CPPUNIT_ASSERT_MESSAGE("Message returned from readMessage null", retMsg.get() != nullptr);
   CPPUNIT_ASSERT_MESSAGE("Message not equal to one pushed", retMsg->getEventId() == testEventId);
}

void QueueTest::test_SendSelfMessage_InvokeProvidedMessage_ExpectingNoThrows()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendSelfMessage failed with a throw", m_TestObject->sendSelfMessage(m_TestMessage));
}

void QueueTest::test_StartTimer_InvokeAfterAlreadyAddingTimer_ExpectingUniqueRefsReturned()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   uint32_t timeout(20);
   uint32_t retRef, retRef1;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("StartTimer failed with a throw", (retRef = m_TestObject->startTimer(m_TestMessage, timeout, true)));
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("StartTimer failed with a throw", (retRef1 = m_TestObject->startTimer(m_TestMessage, timeout, true)));
   CPPUNIT_ASSERT_MESSAGE("Refs returned not unique", retRef != retRef1);

   m_TestObject->stopTimer(retRef);
   m_TestObject->stopTimer(retRef1);
}

void QueueTest::test_StartTimer_InvokeWhenAddingExistingRef_CaseNotTestable()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   uint32_t timeout(20);
   uint32_t retRef, retRef1;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("StartTimer failed with a throw", (retRef = m_TestObject->startTimer(m_TestMessage, timeout, true)));
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   uint32_t exRef = (retRef >> 1) - 1;
   // BUG: TODO
   //   exRef == 0 ? --exRef : exRef;
   exRef <<= 1;
   // Test disabled for performance
   //   while (m_TestObject->makeRef() != exRef);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("StartTimer failed with a throw", (retRef1 = m_TestObject->startTimer(m_TestMessage, timeout, true)));
   CPPUNIT_ASSERT_MESSAGE("Refs returned not unique", retRef != retRef1);

   m_TestObject->stopTimer(retRef);
   m_TestObject->stopTimer(retRef1);
}

void QueueTest::test_StopTimer_InvokeProvidedExistingRefInTimersCyclic_ExpectingTrueReturned()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   uint32_t timeout(100);
   uint32_t retRef;
   retRef = m_TestObject->startTimer(m_TestMessage, timeout, true);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("StopTimer returned false unexpectedly", true, m_TestObject->stopTimer(retRef));
}

void QueueTest::test_StopTimer_InvokeProvidedExistingRefInTimersOneTime_ExpectingTrueReturned()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   uint32_t timeout(1);
   uint32_t retRef;
   retRef = m_TestObject->startTimer(m_TestMessage, timeout, false);
   tsd::common::system::Thread::sleep(timeout);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("StopTimer returned true unexpectedly", false, m_TestObject->stopTimer(retRef));
}

void QueueTest::test_StopTimer_InvokeProvidedNotExistingRefInTimers_ExpectingTrueReturned()
{
   uint32_t retRef = m_TestObject->makeRef();
   uint32_t timeout(1000);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestObject->startTimer(m_TestMessage, timeout, false);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("StopTimer returned false unexpectedly", false, m_TestObject->stopTimer(retRef));
}

void QueueTest::test_GetName_InvokeAfterCreatingWithName_ExpectingNameMatchingWithOnePassedToCtor()
{
   std::string expectedName("testQueueName");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Name not matching one set in ctor", m_TestObject->getName(), expectedName);
}

void QueueTest::test_InterfaceAdded_InvokeProvidedAllArguments_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0);
   IMessageFactoryMock                  testMsgFc;
   std::shared_ptr<IIfcNotifiy>         testNotifier(new IIfcNotifiyMock);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("intefaceAdded failed with a throw",
                                   m_TestObject->interfaceAdded(testAddr, testNotifier.get(), testMsgFc));
}

void QueueTest::test_InterfaceRemoved_InvokeWhenEventWithSameReceiverAddressExistsInEventQueue_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestMessage->setReceiverAddr(testAddr);
   m_TestObject->pushMessage(m_TestMessage, 0);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("interfaceRemovedFailed with a throw", m_TestObject->interfaceRemoved(testAddr));
}

void QueueTest::test_InterfaceRemoved_InvokeWhenEventAddressMismatchesOneInEventQueue_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("interfaceRemovedFailed with a throw", m_TestObject->interfaceRemoved(testAddr));
}

void QueueTest::test_PushMessage_InvokeWhenTimersNotEmptyMulticastTrueNotificationNotNull_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestMessage->setReceiverAddr(testAddr);
   m_TestObject->startTimer(m_TestMessage, 100, true);
   IMessageFactoryMock          testMsgFc;
   IIfcNotifiyMock*             notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy> testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testAddr, testNotifier.get(), testMsgFc);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestMessage->setReceiverAddr(testAddr);
   EXPECT_CALL(*notifiyMock, isSubscribed(_)).WillOnce(Return(true));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("pushMessage failed with a throw", m_TestObject->pushMessage(m_TestMessage, 0, true));

   CPPUNIT_ASSERT_MESSAGE("Verifying and clearing expectations failed", Mock::VerifyAndClearExpectations(notifiyMock));
}

void QueueTest::test_PushMessage_InvokeWhenTimersEmptyMulticastTrueNotificationNull_ExpectingNoThrows()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestMessage->setReceiverAddr(testAddr);
   IMessageFactoryMock          testMsgFc;
   IIfcNotifiyMock*             notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy> testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testAddr, testNotifier.get(), testMsgFc);
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestMessage->setReceiverAddr(testAddr);
   EXPECT_CALL(*notifiyMock, isSubscribed(_)).WillOnce(Return(false));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("pushMessage failed with a throw", m_TestObject->pushMessage(m_TestMessage, 0, true));

   CPPUNIT_ASSERT_MESSAGE("Verifying and clearing expectations failed", Mock::VerifyAndClearExpectations(notifiyMock));
}

class TsdEventNotifyDeleted : public tsd::communication::event::TsdEvent
{
   bool& m_deleted;

public:
   TsdEventNotifyDeleted(bool& deleted) : TsdEvent(1u), m_deleted(deleted)
   {
   }
   ~TsdEventNotifyDeleted()
   {
      m_deleted = true;
   }
};

void QueueTest::test_PushMessage_WithLocalIfcAndMulticastTrue_MessageNotPutIntoQueue()
{
   IMessageFactoryMock        testMsgFc;
   std::string                testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc> ifc(m_TestObject->registerInterface(testMsgFc, testInterfaceName));
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   m_TestMessage->setReceiverAddr(ifc->getLocalIfcAddr());
   IMessageSelectorMock selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(m_TestMessage.get())).Times(0);

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("pushMessage failed with a throw", m_TestObject->pushMessage(m_TestMessage, 0, true));

   m_TestObject->readMessage(1, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectatios shall be met", testing::Mock::VerifyAndClearExpectations(&selector));

   // to increase coverage (and to make sure that there is no memory leak)
   bool                           deleted = false;
   std::auto_ptr<event::TsdEvent> ev(new TsdEventNotifyDeleted(deleted));
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   ifc->monitor(m_TestMessage, ifc->getLocalIfcAddr());
   MonitorRef_t ref = ifc->monitor(ev, ifc->getLocalIfcAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Even should have not been deleted, yet", false, deleted);
   ifc->demonitor(ref);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Even should have been deleted", true, deleted);
}

void QueueTest::test_PushMessage_InvokeWhenTimersEmptyMulticastFalse_ExpectingNoThrows()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("pushMessage failed with a throw", m_TestObject->pushMessage(m_TestMessage, 0, false));
}

void QueueTest::test_PushPacket_InvokeWhenFactoryExistsAndProvidesValidMessage_ExpectingTrueReturned()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   IMessageFactoryMock                  testMsgFc;
   IIfcNotifiyMock*                     notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy>         testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testAddr, testNotifier.get(), testMsgFc);
   tsd::communication::event::TsdEvent  testEvent(1u);
   tsd::communication::event::TsdEvent* testEvent1 = new tsd::communication::event::TsdEvent(1u);
#pragma GCC diagnostic push
#pragma GCC diagnostic   ignored "-Wdeprecated-declarations"
   std::auto_ptr<Packet> testPacket(new Packet(&testEvent, false));
#pragma GCC diagnostic pop
   testPacket->setReceiverAddr(testAddr);
   EXPECT_CALL(testMsgFc, createEventRelay(_)).WillOnce(Return(testEvent1));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("pushPacket returned false unexpectedly", true, m_TestObject->pushPacket(testPacket, false));

   CPPUNIT_ASSERT_MESSAGE("Verifying and clearing expectations failed", &testMsgFc);
}

void QueueTest::test_PushPacket_InvokeWhenFactoryExistsAndProvidesNullMessage_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   IMessageFactoryMock                  testMsgFc;
   IIfcNotifiyMock*                     notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy>         testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testAddr, testNotifier.get(), testMsgFc);
   tsd::communication::event::TsdEvent testEvent(1u);
#pragma GCC diagnostic push
#pragma GCC diagnostic   ignored "-Wdeprecated-declarations"
   std::auto_ptr<Packet> testPacket(new Packet(&testEvent, false));
#pragma GCC diagnostic pop
   testPacket->setReceiverAddr(testAddr);
   EXPECT_CALL(testMsgFc, createEventRelay(_)).WillOnce(Return(nullptr));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("pushPacket returned true unexpectedly", false, m_TestObject->pushPacket(testPacket, false));

   CPPUNIT_ASSERT_MESSAGE("Verifying and clearing expectations failed", &testMsgFc);
}

void QueueTest::test_PushPacket_InvokeWhenFactoryDoesntExist_ExpectingFalseReturned()
{
   tsd::communication::event::IfcAddr_t testAddr(0xFFFFFFFF);
   IMessageFactoryMock                  testMsgFc;
   IIfcNotifiyMock*                     notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy>         testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testAddr, testNotifier.get(), testMsgFc);
   tsd::communication::event::TsdEvent testEvent(1u);
#pragma GCC diagnostic push
#pragma GCC diagnostic   ignored "-Wdeprecated-declarations"
   std::auto_ptr<Packet> testPacket(new Packet(&testEvent, false));
#pragma GCC diagnostic pop
   CPPUNIT_ASSERT_EQUAL_MESSAGE("pushPacket returned true unexpectedly", false, m_TestObject->pushPacket(testPacket, false));
}

void QueueTest::test_PurgeMessages_InvokeProvidedNonZeroRefAnd2ExistingMessages_ExpectingNoThrows()
{
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
#pragma GCC diagnostic push
#pragma GCC diagnostic                                ignored "-Wdeprecated-declarations"
   std::auto_ptr<tsd::communication::event::TsdEvent> testMessage(new tsd::communication::event::TsdEvent(2u));
#pragma GCC diagnostic pop
   m_TestObject->pushMessage(m_TestMessage, 1u);
   m_TestObject->pushMessage(testMessage, 2u);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("purgeMessages failed with a throw", m_TestObject->purgeMessages(2u));
}

void QueueTest::test_PurgeMessages_InvokeProvided0Ref_ExpectingNoThrows()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("purgeMessages failed with a throw", m_TestObject->purgeMessages(0u));
}

void QueueTest::test_GetRouter_InvokeAfterCreatingWithRouter_ExpectingMatchingReferencesOfRouters()
{
   CPPUNIT_ASSERT_MESSAGE("Router reference not matching with one provided in ctor", &(m_TestObject->getRouter()) == m_TestRouter.get());
}

void QueueTest::test_MakeRef_InvokeTwiceThenCompare_ExpectingUniqueReferencesReturned()
{
   uint32_t retRef1, retRef2;
   retRef1 = m_TestObject->makeRef();
   retRef2 = m_TestObject->makeRef();
   CPPUNIT_ASSERT_MESSAGE("makeRef created non unique references", retRef1 != retRef2);
}

void QueueTest::test_Dead_InvokeWhenReceiverAddressMatchesOneInNotifications_ExpectCallOnNotifier()
{
   tsd::communication::event::IfcAddr_t testSenderAddr(0), testReceiveAddr(0xFFFFFFFF);
   IMessageFactoryMock                  testMsgFc;
   IIfcNotifiyMock*                     notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy>         testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testReceiveAddr, testNotifier.get(), testMsgFc);
   EXPECT_CALL(*notifiyMock, notifyDead(_)).Times(1);

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("dead failed with a throw", m_TestObject->dead(testSenderAddr, testReceiveAddr));

   CPPUNIT_ASSERT_MESSAGE("Verifying and clearing expectations failed", notifiyMock);
}

void QueueTest::test_Dead_InvokeWithRealObjects_NotifyDeadExpcted()
{
   IMessageFactoryMock        testMsgFc;
   std::string                testInterfaceName("testInterfaceName");
   std::shared_ptr<ILocalIfc> ifc(m_TestObject->registerInterface(testMsgFc, testInterfaceName));
   std::shared_ptr<ILocalIfc> ifc2(m_TestObject->registerInterface(testMsgFc, testInterfaceName + "2"));
   m_TestMessage.reset(new tsd::communication::event::TsdEvent(1u));
   IMessageSelectorMock selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(m_TestMessage.get())).WillOnce(testing::Return(true));
   ifc->monitor(m_TestMessage, ifc2->getLocalIfcAddr());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("dead failed with a throw", m_TestObject->dead(ifc2->getLocalIfcAddr(), ifc->getLocalIfcAddr()));
   // verify notify dead:
   m_TestObject->readMessage(1, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectatios shall be met", testing::Mock::VerifyAndClearExpectations(&selector));
}

void QueueTest::test_Dead_InvokeWhenReceiverAddressDoesntMatchOneInNotifications_ExpectNoCallsOnNotifier()
{
   tsd::communication::event::IfcAddr_t testSenderAddr(0), testReceiveAddr(0xFFFFFFFF);
   IMessageFactoryMock                  testMsgFc;
   IIfcNotifiyMock*                     notifiyMock = new IIfcNotifiyMock;
   std::shared_ptr<IIfcNotifiy>         testNotifier(notifiyMock);
   m_TestObject->interfaceAdded(testReceiveAddr, testNotifier.get(), testMsgFc);
   EXPECT_CALL(*notifiyMock, notifyDead(_)).Times(0);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("dead failed with a throw", m_TestObject->dead(testSenderAddr, testSenderAddr));

   CPPUNIT_ASSERT_MESSAGE("Verifying and clearing expectations failed", notifiyMock);
}

CPPUNIT_TEST_SUITE_REGISTRATION(QueueTest);

class RemoteIfcMembers
{
public:
   Router              m_Router;
   Queue               m_Queue;
   IMessageFactoryMock m_Factory;
   RemoteIfcMembers() : m_Router("r"), m_Queue("", m_Router)
   {
   }
};

void RemoteIfcTest::setUp()
{
   event::IfcAddr_t addr = 0;
   m_Members             = std::make_shared<RemoteIfcMembers>();
   m_TestObj.reset(m_Members->m_Queue.connectInterface(addr, m_Members->m_Factory));
}

void RemoteIfcTest::verifySubscribed(uint32_t id)
{
   std::auto_ptr<event::TsdEvent> ev(new event::TsdEvent(id));
   ev->setReceiverAddr(m_TestObj->getLocalIfcAddr());
   event::TsdEvent* ev_ptr = ev.get();
   m_Members->m_Queue.pushMessage(ev, 0, true);
   IMessageSelectorMock selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(ev_ptr)).WillOnce(testing::Return(true));
   uint32_t shortTimeout = 1;
   m_Members->m_Queue.readMessage(shortTimeout, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectations shall be met", testing::Mock::VerifyAndClearExpectations(&selector));
}

void RemoteIfcTest::verifyUnsubscribed(uint32_t id)
{
   std::auto_ptr<event::TsdEvent> ev(new event::TsdEvent(id));
   ev->setReceiverAddr(m_TestObj->getLocalIfcAddr());
   event::TsdEvent* ev_ptr = ev.get();
   m_Members->m_Queue.pushMessage(ev, 0, true);
   IMessageSelectorMock selector;
   EXPECT_CALL(selector, filterEvent(testing::_)).WillRepeatedly(testing::Return(false));
   EXPECT_CALL(selector, filterEvent(ev_ptr)).Times(0);
   uint32_t shortTimeout = 1;
   m_Members->m_Queue.readMessage(shortTimeout, &selector);
   CPPUNIT_ASSERT_MESSAGE("All expectations shall be met", testing::Mock::VerifyAndClearExpectations(&selector));
}

void RemoteIfcTest::test_Subscribe_WithVector_Subscribed()
{
   std::vector<uint32_t> events;
   const uint32_t        id1 = 1;
   const uint32_t        id2 = 2;
   events.push_back(id1);
   events.push_back(id2);

   m_TestObj->subscribe(events);

   verifySubscribed(id1);
   verifySubscribed(id2);
}

void RemoteIfcTest::test_Unsubscribe_WithId_Subscribed()
{
   std::vector<uint32_t> events;
   const uint32_t        id1 = 1;
   const uint32_t        id2 = 2;
   events.push_back(id1);
   events.push_back(id2);
   m_TestObj->subscribe(events);

   m_TestObj->unsubscribe(id1);

   verifyUnsubscribed(id1);
   verifySubscribed(id2);
}

void RemoteIfcTest::test_Unsubscribe_WithVector_Subscribed()
{
   std::vector<uint32_t> events;
   const uint32_t        id1 = 1;
   const uint32_t        id2 = 2;
   const uint32_t        id3 = 3;
   events.push_back(id1);
   events.push_back(id2);
   events.push_back(id3);
   m_TestObj->subscribe(events);

   std::vector<uint32_t> unsubscribe;
   unsubscribe.push_back(id1);
   unsubscribe.push_back(id3);
   m_TestObj->unsubscribe(unsubscribe);

   verifyUnsubscribed(id1);
   verifySubscribed(id2);
   verifyUnsubscribed(id3);
}

void RemoteIfcTest::test_Demonitor_JustRun_EventsDestroyed()
{
   bool                           deleted = false;
   std::auto_ptr<event::TsdEvent> ev(new TsdEventNotifyDeleted(deleted));
   ev->setReceiverAddr(m_TestObj->getLocalIfcAddr());
   MonitorRef_t ref = m_TestObj->monitor(ev);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Even should have not been deleted, yet", false, deleted);
   m_TestObj->demonitor(ref);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Even should have been deleted", true, deleted);
}

CPPUNIT_TEST_SUITE_REGISTRATION(RemoteIfcTest);

} // namespace messaging
} // namespace communication
} // namespace tsd
