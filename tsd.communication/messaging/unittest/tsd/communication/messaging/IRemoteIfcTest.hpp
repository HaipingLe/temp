//////////////////////////////////////////////////////////////////////
/// @file IRemoteIfcTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test IRemoteIfc
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_IREMOTEIFCTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_IREMOTEIFCTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for IRemoteIfc
 *
 * @brief Testclass for IRemoteIfc
 */
class IRemoteIfcTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with local ifc addr and remote ifc addr
    *
    * @tsd_testobject tsd::communication::messaging::IRemoteIfc::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithLocalIfcAddrAndRemoteIfcAddr_ObjectCreated();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::IRemoteIfc::GetLocalIfcAddr
    * @tsd_testexpected member local ifc addr returned
    */
   void test_GetLocalIfcAddr_JustRun_MemberLocalIfcAddrReturned();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::IRemoteIfc::GetRemoteIfcAddr
    * @tsd_testexpected member remote ifc addr returned
    */
   void test_GetRemoteIfcAddr_JustRun_MemberRemoteIfcAddrReturned();

   CPPUNIT_TEST_SUITE(IRemoteIfcTest);
   CPPUNIT_TEST(test_Constructor_WithLocalIfcAddrAndRemoteIfcAddr_ObjectCreated);
   CPPUNIT_TEST(test_GetLocalIfcAddr_JustRun_MemberLocalIfcAddrReturned);
   CPPUNIT_TEST(test_GetRemoteIfcAddr_JustRun_MemberRemoteIfcAddrReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_IREMOTEIFCTEST_HPP
