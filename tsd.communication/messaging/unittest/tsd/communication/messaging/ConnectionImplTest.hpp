//////////////////////////////////////////////////////////////////////
/// @file ConnectionImplTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test ConnectionImpl
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTIONIMPLTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTIONIMPLTEST_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <tsd/communication/messaging/IConnectionCallbacksMock.hpp>
#include <tsd/communication/messaging/Router.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for ConnectionImpl
 *
 * @brief Testclass for ConnectionImpl
 */
class ConnectionImplTest : public CPPUNIT_NS::TestFixture
{
public:
   struct PacketHeader
   {
      tsd::common::ipc::NetworkInteger<uint32_t> m_msgLen;
      tsd::common::ipc::NetworkInteger<uint32_t> m_eventId;
      tsd::common::ipc::NetworkInteger<uint64_t> m_senderAddr;
      tsd::common::ipc::NetworkInteger<uint64_t> m_receiverAddr;
      uint8_t                                    m_type;
      uint8_t                                    m_padding[7];
   };

   std::shared_ptr<Router>                   m_Router;
   std::shared_ptr<IConnectionCallbacksMock> m_ConnectionCallbacks;
   std::shared_ptr<ConnectionImpl>           m_TestObj;
   PacketHeader                              m_PacketHeader;

#pragma GCC diagnostic push
#pragma GCC diagnostic   ignored "-Wdeprecated-declarations"
   std::auto_ptr<Packet> m_Packet;
#pragma GCC diagnostic pop
   /**
    * @brief Destructor to clear singletons.
    */
   virtual ~ConnectionImplTest();

   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;
   /**
    * @brief Test scenario: with cb and router
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithCbAndRouter_ObjectCreated();
   /**
    * @brief Test scenario: not ok
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::SetConnected
    * @tsd_testexpected nothing happens
    */
   void test_SetConnected_NotOk_NothingHappens();
   /**
    * @brief Test scenario: ok
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::SetConnected
    * @tsd_testexpected nothing happens
    */
   void test_SetConnected_Ok_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::SetDisconnected
    * @tsd_testexpected nothing happens
    */
   void test_SetDisconnected_JustRun_NothingHappens();
   /**
    * @brief Test scenario: size less than packet header size
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ProcessData
    * @tsd_testexpected data read
    */
   void test_ProcessData_SizeLessThanPacketHeaderSize_DataRead();
   /**
    * @brief Test scenario: size equals packet header and msg len equals packet header size
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ProcessData
    * @tsd_testexpected process packet called
    */
   void test_ProcessData_SizeEqualsPacketHeaderAndMsgLenEqualsPacketHeaderSize_ProcessPacketCalled();
   /**
    * @brief Test scenario: header read but packet still incomplete
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ProcessData
    * @tsd_testexpected data read
    */
   void test_ProcessData_HeaderReadButPacketStillIncomplete_DataRead();
   /**
    * @brief Test scenario: header read and packet data read
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ProcessData
    * @tsd_testexpected process packet called
    */
   void test_ProcessData_HeaderReadAndPacketDataRead_ProcessPacketCalled();
   /**
    * @brief Test scenario: member outgoing pkt equals0
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::GetData
    * @tsd_testexpected false returned
    */
   void test_GetData_MemberOutgoingPktEquals0_FalseReturned();
   /**
    * @brief Test scenario: member outgoing pkt not equals0 and member outgoing offset is less than sizeof
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::GetData
    * @tsd_testexpected true returned
    */
   void test_GetData_MemberOutgoingPktNotEquals0AndMemberOutgoingOffsetIsLessThanSizeof_TrueReturned();
   /**
    * @brief Test scenario: member outgoing pkt not equals0 and member outgoing offset is greater or equal to sizeof
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::GetData
    * @tsd_testexpected true returned
    */
   void test_GetData_MemberOutgoingPktNotEquals0AndMemberOutgoingOffsetIsGreaterOrEqualToSizeof_TrueReturned();
   /**
    * @brief Test scenario: nothing to consume
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ConsumeData
    * @tsd_testexpected nothing happens
    */
   void test_ConsumeData_NothingToConsume_NothingHappens();
   /**
    * @brief Test scenario: single packet with no payload
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ConsumeData
    * @tsd_testexpected nothing happens
    */
   void test_ConsumeData_SinglePacketWithNoPayload_NothingHappens();
   /**
    * @brief Test scenario: packet with payload
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ConsumeData
    * @tsd_testexpected nothing happens
    */
   void test_ConsumeData_PacketWithPayload_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ConnectUpstream
    * @tsd_testexpected false returned
    */
   void test_ConnectUpstream_JustRun_FalseReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::ListenDownstream
    * @tsd_testexpected init downstream returned
    */
   void test_ListenDownstream_JustRun_InitDownstreamReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::Disconnect
    * @tsd_testexpected nothing happens
    */
   void test_Disconnect_JustRun_NothingHappens();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionImpl::Finish
    * @tsd_testexpected nothing happens
    */
   void test_Finish_JustRun_NothingHappens();

   CPPUNIT_TEST_SUITE(ConnectionImplTest);
   CPPUNIT_TEST(test_Constructor_WithCbAndRouter_ObjectCreated);
   CPPUNIT_TEST(test_SetConnected_NotOk_NothingHappens);
   CPPUNIT_TEST(test_SetConnected_Ok_NothingHappens);
   CPPUNIT_TEST(test_SetDisconnected_JustRun_NothingHappens);
   CPPUNIT_TEST(test_ProcessData_SizeLessThanPacketHeaderSize_DataRead);
   CPPUNIT_TEST(test_ProcessData_SizeEqualsPacketHeaderAndMsgLenEqualsPacketHeaderSize_ProcessPacketCalled);
   CPPUNIT_TEST(test_ProcessData_HeaderReadButPacketStillIncomplete_DataRead);
   CPPUNIT_TEST(test_ProcessData_HeaderReadAndPacketDataRead_ProcessPacketCalled);
   CPPUNIT_TEST(test_GetData_MemberOutgoingPktEquals0_FalseReturned);
   CPPUNIT_TEST(test_GetData_MemberOutgoingPktNotEquals0AndMemberOutgoingOffsetIsLessThanSizeof_TrueReturned);
   CPPUNIT_TEST(test_GetData_MemberOutgoingPktNotEquals0AndMemberOutgoingOffsetIsGreaterOrEqualToSizeof_TrueReturned);
   CPPUNIT_TEST(test_ConsumeData_NothingToConsume_NothingHappens);
   CPPUNIT_TEST(test_ConsumeData_SinglePacketWithNoPayload_NothingHappens);
   CPPUNIT_TEST(test_ConsumeData_PacketWithPayload_NothingHappens);
   CPPUNIT_TEST(test_ConnectUpstream_JustRun_FalseReturned);
   CPPUNIT_TEST(test_ListenDownstream_JustRun_InitDownstreamReturned);
   CPPUNIT_TEST(test_Disconnect_JustRun_NothingHappens);
   CPPUNIT_TEST(test_Finish_JustRun_NothingHappens);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_CONNECTIONIMPLTEST_HPP
