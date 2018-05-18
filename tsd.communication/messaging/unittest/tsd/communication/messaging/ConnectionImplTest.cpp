//////////////////////////////////////////////////////////////////////
/// @file ConnectionImplTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test ConnectionImpl
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "ConnectionImplTest.hpp"
#include <gmock/gmock.h>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/ConnectionImpl.hpp>
#include <tsd/communication/messaging/IPort.hpp>
#include <tsd/communication/messaging/Packet.hpp>
#include <vector>

namespace tsd {
namespace communication {
namespace messaging {

ConnectionImplTest::~ConnectionImplTest()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void ConnectionImplTest::setUp()
{
   m_Router              = std::make_shared<Router>("TheRouter");
   m_ConnectionCallbacks = std::make_shared<IConnectionCallbacksMock>();
   m_TestObj             = std::make_shared<ConnectionImpl>(*m_ConnectionCallbacks, *m_Router);

   m_PacketHeader = {.m_msgLen       = sizeof(m_PacketHeader),
                     .m_eventId      = 0,
                     .m_senderAddr   = 0,
                     .m_receiverAddr = 0,
                     .m_type         = 0,
                     .m_padding      = {0, 0, 0, 0, 0, 0, 0}};
}

void ConnectionImplTest::tearDown()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Verify expectations failed", testing::Mock::VerifyAndClear(m_ConnectionCallbacks.get()));
}

void ConnectionImplTest::test_Constructor_WithCbAndRouter_ObjectCreated()
{
   Router                   router("TheRouter");
   IConnectionCallbacksMock cb;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", std::make_shared<ConnectionImpl>(cb, router));
}

void ConnectionImplTest::test_SetConnected_NotOk_NothingHappens()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());
}

void ConnectionImplTest::test_SetConnected_Ok_NothingHappens()
{
   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(true));
   EXPECT_CALL(*m_ConnectionCallbacks, wakeup());
   reinterpret_cast<IPort*>(m_TestObj.get())->initDownstream();
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());
}

void ConnectionImplTest::test_SetDisconnected_JustRun_NothingHappens()
{
   // It is necessary to connect first. Otherwise assertion in IPort::disconnected() will not be met.
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setDisconnected failed", m_TestObj->setDisconnected());
}

void ConnectionImplTest::test_ProcessData_SizeLessThanPacketHeaderSize_DataRead()
{
   std::vector<uint8_t> packet(1);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", m_TestObj->processData(packet.data(), packet.size()));
}

void ConnectionImplTest::test_ProcessData_SizeEqualsPacketHeaderAndMsgLenEqualsPacketHeaderSize_ProcessPacketCalled()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", m_TestObj->processData(&m_PacketHeader, sizeof(PacketHeader)));
}

void ConnectionImplTest::test_ProcessData_HeaderReadButPacketStillIncomplete_DataRead()
{
   m_PacketHeader.m_msgLen = sizeof(PacketHeader) + 2ull;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", m_TestObj->processData(&m_PacketHeader, sizeof(PacketHeader)));
   std::vector<uint8_t> packet(1);
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", m_TestObj->processData(packet.data(), packet.size()));
}

void ConnectionImplTest::test_ProcessData_HeaderReadAndPacketDataRead_ProcessPacketCalled()
{
   m_PacketHeader.m_msgLen = 2ull;
   std::vector<uint8_t> packet;
   packet.push_back(0);

   // It is necessary to connect first. Otherwise assertion in IPort::processData() will not be met.
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", (m_TestObj->processData(&m_PacketHeader, sizeof(PacketHeader))));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", (m_TestObj->processData(packet.data(), packet.size())));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("processData failed", (m_TestObj->processData(packet.data(), packet.size())));
}

void ConnectionImplTest::test_GetData_MemberOutgoingPktEquals0_FalseReturned()
{
   const void* data;
   size_t      size;
   CPPUNIT_ASSERT_EQUAL_MESSAGE("getData returned wrong value", false, m_TestObj->getData(data, size));
}

void ConnectionImplTest::test_GetData_MemberOutgoingPktNotEquals0AndMemberOutgoingOffsetIsLessThanSizeof_TrueReturned()
{
   IPort* port = reinterpret_cast<IPort*>(m_TestObj.get());

   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(true));
   EXPECT_CALL(*m_ConnectionCallbacks, wakeup());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initDownstream failed", port->initDownstream());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());

   tsd::communication::event::IfcAddr_t src{0ull};
   tsd::communication::event::IfcAddr_t dst{0ull};
   m_Packet.reset(new Packet(Packet::Type::UNICAST_MESSAGE, src, dst));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendPacket failed", port->sendPacket(m_Packet));

   const void* data;
   size_t      size;
   CPPUNIT_ASSERT_EQUAL_MESSAGE("getData returned wrong value", true, m_TestObj->getData(data, size));
}

void ConnectionImplTest::test_GetData_MemberOutgoingPktNotEquals0AndMemberOutgoingOffsetIsGreaterOrEqualToSizeof_TrueReturned()
{
   IPort* port = reinterpret_cast<IPort*>(m_TestObj.get());

   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(true));
   EXPECT_CALL(*m_ConnectionCallbacks, wakeup());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initDownstream failed", port->initDownstream());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());

   tsd::communication::event::IfcAddr_t src{0ull};
   tsd::communication::event::IfcAddr_t dst{0ull};
   char                                 buffer[10];
   m_Packet.reset(new Packet(Packet::Type::UNICAST_MESSAGE, src, dst, 0, buffer, sizeof(buffer)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendPacket failed", port->sendPacket(m_Packet));
   m_TestObj->consumeData(sizeof(PacketHeader));

   const void* data;
   size_t      size;
   CPPUNIT_ASSERT_EQUAL_MESSAGE("getData returned wrong value", true, (m_TestObj->getData(data, size)));
}

void ConnectionImplTest::test_ConsumeData_NothingToConsume_NothingHappens()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("consumeData failed", m_TestObj->consumeData(1));
}

void ConnectionImplTest::test_ConsumeData_SinglePacketWithNoPayload_NothingHappens()
{
   IPort* port = reinterpret_cast<IPort*>(m_TestObj.get());

   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(true));
   EXPECT_CALL(*m_ConnectionCallbacks, wakeup());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initDownstream failed", port->initDownstream());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());

   tsd::communication::event::IfcAddr_t src{0ull};
   tsd::communication::event::IfcAddr_t dst{0ull};
   m_Packet.reset(new Packet(Packet::Type::UNICAST_MESSAGE, src, dst));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendPacket failed", port->sendPacket(m_Packet));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("consumeData failed", m_TestObj->consumeData(sizeof(PacketHeader)));
}

void ConnectionImplTest::test_ConsumeData_PacketWithPayload_NothingHappens()
{
   IPort* port = reinterpret_cast<IPort*>(m_TestObj.get());

   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(true));
   EXPECT_CALL(*m_ConnectionCallbacks, wakeup());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initDownstream failed", port->initDownstream());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());

   tsd::communication::event::IfcAddr_t src{0ull};
   tsd::communication::event::IfcAddr_t dst{0ull};
   char                                 buffer[10];

   m_Packet.reset(new Packet(Packet::Type::UNICAST_MESSAGE, src, dst, 0, buffer, sizeof(buffer)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendPacket failed", port->sendPacket(m_Packet));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendPacket failed", m_TestObj->consumeData(sizeof(PacketHeader)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("sendPacket failed", m_TestObj->consumeData(24));
}

void ConnectionImplTest::test_ConnectUpstream_JustRun_FalseReturned()
{
   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(false));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("connectUpstream returned wrong value", false, (m_TestObj->connectUpstream("subDomain")));
}

void ConnectionImplTest::test_ListenDownstream_JustRun_InitDownstreamReturned()
{
   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(false));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("listenDownstream returned wrong value", m_TestObj->listenDownstream());
}

void ConnectionImplTest::test_Disconnect_JustRun_NothingHappens()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("disconnect failed", m_TestObj->disconnect());
}

void ConnectionImplTest::test_Finish_JustRun_NothingHappens()
{
   IPort* port = reinterpret_cast<IPort*>(m_TestObj.get());

   EXPECT_CALL(*m_ConnectionCallbacks, setupConnection()).WillOnce(testing::Return(true));
   EXPECT_CALL(*m_ConnectionCallbacks, wakeup());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("initDownstream failed", port->initDownstream());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("setConnected failed", m_TestObj->setConnected());
   EXPECT_CALL(*m_ConnectionCallbacks, teardownConnection());
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("finish failed", port->finish());
}

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionImplTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
