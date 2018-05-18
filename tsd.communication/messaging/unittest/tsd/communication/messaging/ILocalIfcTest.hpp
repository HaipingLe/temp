//////////////////////////////////////////////////////////////////////
/// @file ILocalIfcTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test ILocalIfc
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_ILOCALIFCTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_ILOCALIFCTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for ILocalIfc
 *
 * @brief Testclass for ILocalIfc
 */
class ILocalIfcTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with local ifc addr
    *
    * @tsd_testobject tsd::communication::messaging::ILocalIfc::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithLocalIfcAddr_ObjectCreated();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::ILocalIfc::GetLocalIfcAddr
    * @tsd_testexpected member local ifc addr returned
    */
   void test_GetLocalIfcAddr_JustRun_MemberLocalIfcAddrReturned();

   CPPUNIT_TEST_SUITE(ILocalIfcTest);
   CPPUNIT_TEST(test_Constructor_WithLocalIfcAddr_ObjectCreated);
   CPPUNIT_TEST(test_GetLocalIfcAddr_JustRun_MemberLocalIfcAddrReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_ILOCALIFCTEST_HPP
