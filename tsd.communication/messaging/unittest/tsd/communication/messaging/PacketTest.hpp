//////////////////////////////////////////////////////////////////////
/// @file PacketTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test Packet
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_PACKETTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_PACKETTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for Packet
 *
 * @brief Testclass for Packet
 */
class PacketTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with obj
    *
    * @tsd_testobject tsd::communication::messaging::Packet::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithObj_ObjectCreated();
   /**
    * @brief Test scenario: with msg and multicast set to false
    *
    * @tsd_testobject tsd::communication::messaging::Packet::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithMsgAndMulticastSetToFalse_ObjectCreated();
   /**
    * @brief Test scenario: with type and sender and receiver
    *
    * @tsd_testobject tsd::communication::messaging::Packet::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithTypeAndSenderAndReceiver_ObjectCreated();
   /**
    * @brief Test scenario: with msg and multicast set to true
    *
    * @tsd_testobject tsd::communication::messaging::Packet::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithMsgAndMulticastSetToTrue_ObjectCreated();
   /**
    * @brief Test scenario: with type and sender and receiver and event id and buffer and buffer length
    *
    * @tsd_testobject tsd::communication::messaging::Packet::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithTypeAndSenderAndReceiverAndEventIdAndBufferAndBufferLength_ObjectCreated();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::GetType
    * @tsd_testexpected member type returned
    */
   void test_GetType_JustRun_MemberTypeReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::GetSenderAddr
    * @tsd_testexpected member sender addr returned
    */
   void test_GetSenderAddr_JustRun_MemberSenderAddrReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::SetSenderAddr
    * @tsd_testexpected nothing happens
    */
   void test_SetSenderAddr_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::GetReceiverAddr
    * @tsd_testexpected member receiver addr returned
    */
   void test_GetReceiverAddr_JustRun_MemberReceiverAddrReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::SetReceiverAddr
    * @tsd_testexpected nothing happens
    */
   void test_SetReceiverAddr_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::GetEventId
    * @tsd_testexpected member event id returned
    */
   void test_GetEventId_JustRun_MemberEventIdReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::GetBufferLength
    * @tsd_testexpected member buffer size returned
    */
   void test_GetBufferLength_JustRun_MemberBufferSizeReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Packet::GetBufferPtr
    * @tsd_testexpected bit and member buffer returned
    */
   void test_GetBufferPtr_JustRun_BitAndMemberBufferReturned();

   CPPUNIT_TEST_SUITE(PacketTest);
   CPPUNIT_TEST(test_Constructor_WithObj_ObjectCreated);
   CPPUNIT_TEST(test_Constructor_WithMsgAndMulticastSetToFalse_ObjectCreated);
   CPPUNIT_TEST(test_Constructor_WithMsgAndMulticastSetToTrue_ObjectCreated);
   CPPUNIT_TEST(test_Constructor_WithTypeAndSenderAndReceiver_ObjectCreated);
   CPPUNIT_TEST(test_Constructor_WithTypeAndSenderAndReceiverAndEventIdAndBufferAndBufferLength_ObjectCreated);
   CPPUNIT_TEST(test_GetType_JustRun_MemberTypeReturned);
   CPPUNIT_TEST(test_GetSenderAddr_JustRun_MemberSenderAddrReturned);
   CPPUNIT_TEST(test_SetSenderAddr_JustRun_NothingHappens);
   CPPUNIT_TEST(test_GetReceiverAddr_JustRun_MemberReceiverAddrReturned);
   CPPUNIT_TEST(test_SetReceiverAddr_JustRun_NothingHappens);
   CPPUNIT_TEST(test_GetEventId_JustRun_MemberEventIdReturned);
   CPPUNIT_TEST(test_GetBufferLength_JustRun_MemberBufferSizeReturned);
   CPPUNIT_TEST(test_GetBufferPtr_JustRun_BitAndMemberBufferReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_PACKETTEST_HPP
