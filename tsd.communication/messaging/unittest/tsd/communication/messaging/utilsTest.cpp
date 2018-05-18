//////////////////////////////////////////////////////////////////////
/// @file utilsTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test utils
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "utilsTest.hpp"
#include <bitset>
#include <tsd/communication/messaging/utils.hpp>
namespace tsd {
namespace communication {
namespace messaging {

void GlobalutilsTest::test_GetHostAddr_JustRun_AddrBitAndHostAddrMaskReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{63u};
   const event::IfcAddr_t               expectedResult{addr & HOST_ADDR_MASK};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returend value is expected to be equal to expectedResult", expectedResult, getHostAddr(addr));
}

void GlobalutilsTest::test_GetInterface_JustRun_AddrBitAndInterfaceAddrMaskReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{63u};
   const event::IfcAddr_t               expectedResult{addr & INTERFACE_ADDR_MASK};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returend value is expected to be equal to expectedResult", expectedResult, getInterface(addr));
}

void GlobalutilsTest::test_IsHostAddr_GetHostAddrNotEqualsGetHostAddr_FalseReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{~0u};
   tsd::communication::event::IfcAddr_t host{0u};
   const bool                           expectedResult{false};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, getHostAddr(addr) == getHostAddr(host));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, isHostAddr(addr, host));
}

void GlobalutilsTest::test_IsHostAddr_GetHostAddrEqualsGetHostAddr_TrueReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{0u};
   tsd::communication::event::IfcAddr_t host{0u};
   const bool                           expectedResult{true};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, getHostAddr(addr) == getHostAddr(host));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, isHostAddr(addr, host));
}

void GlobalutilsTest::test_IsLocalAddr_JustRun_IsHostAddrReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{0u};
   const bool                           expectedResult = isHostAddr(addr, 0);

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, isLocalAddr(addr));
}

void GlobalutilsTest::test_IsNetworkAddr_AddrBitAndMaskNotEqualsNetBitAndMask_FalseReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{1u};
   tsd::communication::event::IfcAddr_t net{2u};
   tsd::communication::event::IfcAddr_t mask{3u};
   const bool                           expectedResult{false};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, (addr & mask) == (net & mask));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, isNetworkAddr(addr, net, mask));
}

void GlobalutilsTest::test_IsNetworkAddr_AddrBitAndMaskEqualsNetBitAndMask_TrueReturned()
{
   // function under test parameter preparation:
   tsd::communication::event::IfcAddr_t addr{1u};
   tsd::communication::event::IfcAddr_t net{2u};
   tsd::communication::event::IfcAddr_t mask{4u};
   const bool                           expectedResult{true};

   // test execution
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, (addr & mask) == (net & mask));
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Equal values eqpected", expectedResult, isNetworkAddr(addr, net, mask));
}

CPPUNIT_TEST_SUITE_REGISTRATION(GlobalutilsTest);

} // namespace messaging
} // namespace communication
} // namespace tsd
