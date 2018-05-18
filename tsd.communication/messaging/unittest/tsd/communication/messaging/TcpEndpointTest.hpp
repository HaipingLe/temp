//////////////////////////////////////////////////////////////////////
/// @file TcpEndpointTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test TcpEndpoint
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_TCPENDPOINTTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPENDPOINTTEST_HPP

#include <arpa/inet.h>
#include <chrono>
#include <cppunit/extensions/HelperMacros.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <tsd/common/logging/Logger.hpp>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/TcpEndpointMock.hpp>

namespace tsd {
namespace communication {
namespace messaging {

class Select;
class Packet;

/**
 * Testclass for TcpEndpoint
 *
 * @brief Testclass for TcpEndpoint
 */
class TcpEndpointTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Destructor to clear singletons.
    */
   virtual ~TcpEndpointTest();
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;
   /**
    * @brief Test scenario: with empty send queue and zero offset
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::EpSendPacket
    * @tsd_testexpected true returned
    */
   void test_EpSendPacket_WithEmptySendQueueAndZeroOffset_TrueReturned();
   /**
    * @brief Test scenario: call twice with zero offset
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::EpSendPacket
    * @tsd_testexpected true returned
    */
   void test_EpSendPacket_CallTwiceWithZeroOffset_TrueReturned();
   /**
    * @brief Test scenario: with negative offset
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::EpSendPacket
    * @tsd_testexpected true returned
    */
   void test_EpSendPacket_WithNegativeOffset_TrueReturned();
   /**
    * @brief Test scenario: with not empty buffer lenght
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::EpSendPacket
    * @tsd_testexpected true returned
    */
   void test_EpSendPacket_WithNotEmptyBufferLenght_TrueReturned();
   /**
    * @brief Test scenario: run twice
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::Init
    * @tsd_testexpected returned false twice
    */
   void test_Init_RunTwice_ReturnedFalseTwice();
   /**
    * @brief Test scenario: select null and socket invalid
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::Cleanup
    * @tsd_testexpected nothing happens
    */
   void test_Cleanup_SelectNullAndSocketInvalid_NothingHappens();
   /**
    * @brief Test scenario: select not null and socket valid
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::Cleanup
    * @tsd_testexpected nothing happens
    */
   void test_Cleanup_SelectNotNullAndSocketValid_NothingHappens();
   /**
    * @brief Test scenario: with negative offset
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::SelectWritable
    * @tsd_testexpected no exception thrown
    */
   void test_SelectWritable_WithNegativeOffset_NoExceptionThrown();
   /**
    * @brief Test scenario: with not empty message
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::SelectReadable
    * @tsd_testexpected no exception thrown
    */
   void test_SelectReadable_WithNotEmptyMessage_NoExceptionThrown();
   /**
    * @brief Test scenario: with negative offset
    *
    * @tsd_testobject tsd::communication::messaging::TcpEndpoint::SelectError
    * @tsd_testexpected no exception thrown
    */
   void test_SelectError_WithNegativeOffset_NoExceptionThrown();

   CPPUNIT_TEST_SUITE(TcpEndpointTest);
   CPPUNIT_TEST(test_EpSendPacket_WithEmptySendQueueAndZeroOffset_TrueReturned);
   CPPUNIT_TEST(test_EpSendPacket_CallTwiceWithZeroOffset_TrueReturned);
   CPPUNIT_TEST(test_EpSendPacket_WithNegativeOffset_TrueReturned);
   CPPUNIT_TEST(test_EpSendPacket_WithNotEmptyBufferLenght_TrueReturned);
   CPPUNIT_TEST(test_Init_RunTwice_ReturnedFalseTwice);
   CPPUNIT_TEST(test_Cleanup_SelectNullAndSocketInvalid_NothingHappens);
   CPPUNIT_TEST(test_Cleanup_SelectNotNullAndSocketValid_NothingHappens);
   CPPUNIT_TEST(test_SelectWritable_WithNegativeOffset_NoExceptionThrown);
   CPPUNIT_TEST(test_SelectReadable_WithNotEmptyMessage_NoExceptionThrown);
   CPPUNIT_TEST(test_SelectError_WithNegativeOffset_NoExceptionThrown);
   CPPUNIT_TEST_SUITE_END();

private:
   std::shared_ptr<TcpEndpointMock>                     m_TestObj;
   std::shared_ptr<tsd::common::logging::Logger>        m_Logger;
   std::shared_ptr<Select>                              m_Select;
   std::shared_ptr<tsd::communication::event::TsdEvent> m_Event;
   int                                                  m_SockFd;
   struct sockaddr_in                                   m_ServAddr;
#pragma GCC diagnostic push
#pragma GCC diagnostic   ignored "-Wdeprecated-declarations"
   std::auto_ptr<Packet> m_Packet;
#pragma GCC diagnostic pop
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_TCPENDPOINTTEST_HPP
