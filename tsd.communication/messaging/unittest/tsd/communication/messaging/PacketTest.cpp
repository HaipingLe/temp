//////////////////////////////////////////////////////////////////////
/// @file PacketTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test Packet
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "PacketTest.hpp"
#include <tsd/communication/messaging/Packet.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
const uint32_t                             DEFAULT_EVENT_ID{1};
const Packet::Type                         DEFAULT_TYPE{Packet::OOB_BASE};
const tsd::communication::event::IfcAddr_t DEFAULT_SENDER{1};
const tsd::communication::event::IfcAddr_t DEFAULT_RECEIVER{2};
const char*                                DEFAULT_BUFFER{"testbuffer"};
}

void PacketTest::test_Constructor_WithObj_ObjectCreated()
{
   tsd::communication::event::TsdEvent msg{DEFAULT_EVENT_ID};
   Packet                              obj(&msg, false);
   std::shared_ptr<Packet>             packet;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", packet = std::make_shared<Packet>(obj));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", obj.getSenderAddr(), packet->getSenderAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", obj.getReceiverAddr(), packet->getReceiverAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id address is not as expected", obj.getEventId(), packet->getEventId());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", obj.getBufferLength(), packet->getBufferLength());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Type is not as expected", obj.getType(), packet->getType());
}

void PacketTest::test_Constructor_WithMsgAndMulticastSetToFalse_ObjectCreated()
{
   tsd::communication::event::TsdEvent msg{DEFAULT_EVENT_ID};
   const bool                          multicast = false;
   std::shared_ptr<Packet>             packet;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", packet = std::make_shared<Packet>(&msg, multicast));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", msg.getSenderAddr(), packet->getSenderAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", msg.getReceiverAddr(), packet->getReceiverAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id address is not as expected", msg.getEventId(), packet->getEventId());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", size_t{0}, packet->getBufferLength());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Type is not as expected", Packet::UNICAST_MESSAGE, packet->getType());
}

void PacketTest::test_Constructor_WithMsgAndMulticastSetToTrue_ObjectCreated()
{
   tsd::communication::event::TsdEvent msg{DEFAULT_EVENT_ID};
   const bool                          multicast = true;
   std::shared_ptr<Packet>             packet;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", packet = std::make_shared<Packet>(&msg, multicast));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", msg.getSenderAddr(), packet->getSenderAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", msg.getReceiverAddr(), packet->getReceiverAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id address is not as expected", msg.getEventId(), packet->getEventId());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", size_t{0}, packet->getBufferLength());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Type is not as expected", Packet::MULTICAST_MESSAGE, packet->getType());
}

void PacketTest::test_Constructor_WithTypeAndSenderAndReceiver_ObjectCreated()
{
   std::shared_ptr<Packet> packet;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw",
                                   packet = std::make_shared<Packet>(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", DEFAULT_SENDER, packet->getSenderAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", DEFAULT_RECEIVER, packet->getReceiverAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id address is not as expected", uint32_t{0}, packet->getEventId());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", size_t{0}, packet->getBufferLength());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Type is not as expected", DEFAULT_TYPE, packet->getType());
}

void PacketTest::test_Constructor_WithTypeAndSenderAndReceiverAndEventIdAndBufferAndBufferLength_ObjectCreated()
{
   std::shared_ptr<Packet> packet;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE(
      "Constructor should not throw",
      packet = std::make_shared<Packet>(
         DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER, DEFAULT_EVENT_ID, DEFAULT_BUFFER, strlen(DEFAULT_BUFFER)));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", DEFAULT_SENDER, packet->getSenderAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", DEFAULT_RECEIVER, packet->getReceiverAddr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id address is not as expected", DEFAULT_EVENT_ID, packet->getEventId());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", strlen(DEFAULT_BUFFER), packet->getBufferLength());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", DEFAULT_BUFFER[0], *packet->getBufferPtr());
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Type is not as expected", DEFAULT_TYPE, packet->getType());
}

void PacketTest::test_GetType_JustRun_MemberTypeReturned()
{
   Packet packet(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Type is not as expected", DEFAULT_TYPE, packet.getType());
}

void PacketTest::test_GetSenderAddr_JustRun_MemberSenderAddrReturned()
{
   Packet packet(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", DEFAULT_SENDER, packet.getSenderAddr());
}

void PacketTest::test_SetSenderAddr_JustRun_NothingHappens()
{
   Packet                               packet(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER);
   tsd::communication::event::IfcAddr_t newSenderAddr{3};
   packet.setSenderAddr(newSenderAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Sender address is not as expected", newSenderAddr, packet.getSenderAddr());
}

void PacketTest::test_GetReceiverAddr_JustRun_MemberReceiverAddrReturned()
{
   Packet packet(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", DEFAULT_RECEIVER, packet.getReceiverAddr());
}

void PacketTest::test_SetReceiverAddr_JustRun_NothingHappens()
{
   Packet                               packet(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER);
   tsd::communication::event::IfcAddr_t newReceiverAddr{3};
   packet.setReceiverAddr(newReceiverAddr);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Receiver address is not as expected", newReceiverAddr, packet.getReceiverAddr());
}

void PacketTest::test_GetEventId_JustRun_MemberEventIdReturned()
{
   std::shared_ptr<Packet> packet =
      std::make_shared<Packet>(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER, DEFAULT_EVENT_ID, DEFAULT_BUFFER, strlen(DEFAULT_BUFFER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Event id address is not as expected", DEFAULT_EVENT_ID, packet->getEventId());
}

void PacketTest::test_GetBufferLength_JustRun_MemberBufferSizeReturned()
{
   std::shared_ptr<Packet> packet =
      std::make_shared<Packet>(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER, DEFAULT_EVENT_ID, DEFAULT_BUFFER, strlen(DEFAULT_BUFFER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", strlen(DEFAULT_BUFFER), packet->getBufferLength());
}

void PacketTest::test_GetBufferPtr_JustRun_BitAndMemberBufferReturned()
{
   std::shared_ptr<Packet> packet =
      std::make_shared<Packet>(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER, DEFAULT_EVENT_ID, DEFAULT_BUFFER, strlen(DEFAULT_BUFFER));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Buffer length is not as expected", DEFAULT_BUFFER[0], *packet->getBufferPtr());
}

CPPUNIT_TEST_SUITE_REGISTRATION(PacketTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
