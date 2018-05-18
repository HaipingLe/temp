//////////////////////////////////////////////////////////////////////
/// @file SelectTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test Select
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_SELECTTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_SELECTTEST_HPP

#include <cppunit/extensions/HelperMacros.h>
#include <memory>
#include <tsd/communication/messaging/ISelectEventHandlerMock.hpp>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for Select
 *
 * @brief Testclass for Select
 */
class SelectTest : public CPPUNIT_NS::TestFixture
{
public:
   std::shared_ptr<Select>                  m_TestObj;
   std::shared_ptr<ISelectEventHandlerMock> m_SelectEventHandlerMock;
   struct sockaddr_in                       m_SiMe;
   socklen_t                                m_SiMeLen;
   int                                      m_UdpSocket;
   uint8_t                                  m_Buffer[128];
   /**
    * @brief setUp function called before every test
    */
   void setUp() override;
   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;
   /**
    * @brief Test scenario: without parameters
    *
    * @tsd_testobject tsd::communication::messaging::Select::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithoutParameters_ObjectCreated();
   /**
    * @brief Test scenario: again same listener
    *
    * @tsd_testobject tsd::communication::messaging::Select::Add
    * @tsd_testexpected nothing happens
    */
   void test_Add_AgainSameListener_NothingHappens();
   /**
    * @brief Test scenario: all fds created and registered
    *
    * @tsd_testobject tsd::communication::messaging::Select::Init
    * @tsd_testexpected true returned
    */
   void test_Init_AllFdsCreatedAndRegistered_TrueReturned();
   /**
    * @brief Test scenario: not enough descriptors for mwake fd
    *
    * @tsd_testobject tsd::communication::messaging::Select::Init
    * @tsd_testexpected false returned
    */
   void test_Init_NotEnoughDescriptorsForMwakeFd_FalseReturned();
   /**
    * @brief Test scenario: not enough descriptors
    *
    * @tsd_testobject tsd::communication::messaging::Select::Init
    * @tsd_testexpected false returned
    */
   void test_Init_NotEnoughDescriptors_FalseReturned();
   /**
    * @brief Test scenario: epoll wait failed
    *
    * @tsd_testobject tsd::communication::messaging::Select::Dispatch
    * @tsd_testexpected nothing happened
    */
   void test_Dispatch_EpollWaitFailed_NothingHappened();
   /**
    * @brief Test scenario: wait for writeable
    *
    * @tsd_testobject tsd::communication::messaging::Select::Dispatch
    * @tsd_testexpected select writeable called
    */
   void test_Dispatch_WaitForWriteable_SelectWriteableCalled();
   /**
    * @brief Test scenario: wait for readable
    *
    * @tsd_testobject tsd::communication::messaging::Select::Dispatch
    * @tsd_testexpected select readable called
    */
   void test_Dispatch_WaitForReadable_SelectReadableCalled();
   /**
    * @brief Test scenario: wait for hang up
    *
    * @tsd_testobject tsd::communication::messaging::Select::Dispatch
    * @tsd_testexpected select error called
    */
   void test_Dispatch_WaitForHangUp_SelectErrorCalled();
   /**
    * @brief Test scenario: wait for writeable and readable readable received
    *
    * @tsd_testobject tsd::communication::messaging::Select::Dispatch
    * @tsd_testexpected expectations modified
    */
   void test_Dispatch_WaitForWriteableAndReadableReadableReceived_ExpectationsModified();

   CPPUNIT_TEST_SUITE(SelectTest);
   CPPUNIT_TEST(test_Constructor_WithoutParameters_ObjectCreated);
   CPPUNIT_TEST(test_Add_AgainSameListener_NothingHappens);
   CPPUNIT_TEST(test_Init_AllFdsCreatedAndRegistered_TrueReturned);
   CPPUNIT_TEST(test_Init_NotEnoughDescriptorsForMwakeFd_FalseReturned);
   CPPUNIT_TEST(test_Init_NotEnoughDescriptors_FalseReturned);
   CPPUNIT_TEST(test_Dispatch_EpollWaitFailed_NothingHappened);
   CPPUNIT_TEST(test_Dispatch_WaitForWriteable_SelectWriteableCalled);
   CPPUNIT_TEST(test_Dispatch_WaitForReadable_SelectReadableCalled);
   CPPUNIT_TEST(test_Dispatch_WaitForHangUp_SelectErrorCalled);
   CPPUNIT_TEST(test_Dispatch_WaitForWriteableAndReadableReadableReceived_ExpectationsModified);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_SELECTTEST_HPP
