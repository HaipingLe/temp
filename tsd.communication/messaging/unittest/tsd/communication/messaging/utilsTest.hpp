//////////////////////////////////////////////////////////////////////
/// @file utilsTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test utils
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_UTILSTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_UTILSTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for Globalutils
 *
 * @brief Testclass for Globalutils
 */
class GlobalutilsTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::GetHostAddr
    * @tsd_testexpected addr bit and host addr mask returned
    */
   void test_GetHostAddr_JustRun_AddrBitAndHostAddrMaskReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::GetInterface
    * @tsd_testexpected addr bit and interface addr mask returned
    */
   void test_GetInterface_JustRun_AddrBitAndInterfaceAddrMaskReturned();
   /**
    * @brief Test scenario: get host addr not equals get host addr
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::IsHostAddr
    * @tsd_testexpected false returned
    */
   void test_IsHostAddr_GetHostAddrNotEqualsGetHostAddr_FalseReturned();
   /**
    * @brief Test scenario: get host addr equals get host addr
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::IsHostAddr
    * @tsd_testexpected true returned
    */
   void test_IsHostAddr_GetHostAddrEqualsGetHostAddr_TrueReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::IsLocalAddr
    * @tsd_testexpected is host addr returned
    */
   void test_IsLocalAddr_JustRun_IsHostAddrReturned();
   /**
    * @brief Test scenario: addr bit and mask not equals net bit and mask
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::IsNetworkAddr
    * @tsd_testexpected false returned
    */
   void test_IsNetworkAddr_AddrBitAndMaskNotEqualsNetBitAndMask_FalseReturned();
   /**
    * @brief Test scenario: addr bit and mask equals net bit and mask
    *
    * @tsd_testobject tsd::communication::messaging::Globalutils::IsNetworkAddr
    * @tsd_testexpected true returned
    */
   void test_IsNetworkAddr_AddrBitAndMaskEqualsNetBitAndMask_TrueReturned();

   CPPUNIT_TEST_SUITE(GlobalutilsTest);
   CPPUNIT_TEST(test_GetHostAddr_JustRun_AddrBitAndHostAddrMaskReturned);
   CPPUNIT_TEST(test_GetInterface_JustRun_AddrBitAndInterfaceAddrMaskReturned);
   CPPUNIT_TEST(test_IsHostAddr_GetHostAddrNotEqualsGetHostAddr_FalseReturned);
   CPPUNIT_TEST(test_IsHostAddr_GetHostAddrEqualsGetHostAddr_TrueReturned);
   CPPUNIT_TEST(test_IsLocalAddr_JustRun_IsHostAddrReturned);
   CPPUNIT_TEST(test_IsNetworkAddr_AddrBitAndMaskNotEqualsNetBitAndMask_FalseReturned);
   CPPUNIT_TEST(test_IsNetworkAddr_AddrBitAndMaskEqualsNetBitAndMask_TrueReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_UTILSTEST_HPP
