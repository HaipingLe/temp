//////////////////////////////////////////////////////////////////////
/// @file ILocalIfcTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test ILocalIfc
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "ILocalIfcTest.hpp"
#include <tsd/communication/messaging/ILocalIfc.hpp>
#include <tsd/communication/messaging/ILocalIfcMock.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
const tsd::communication::event::IfcAddr_t DEFAULT_ADDR{1};
}

void ILocalIfcTest::test_Constructor_WithLocalIfcAddr_ObjectCreated()
{
   std::unique_ptr<ILocalIfc> localIfc;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", localIfc.reset(new ILocalIfcMock(DEFAULT_ADDR)));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Destructor should not throw", localIfc.reset());
}

void ILocalIfcTest::test_GetLocalIfcAddr_JustRun_MemberLocalIfcAddrReturned()
{
   ILocalIfcMock localIfc(DEFAULT_ADDR);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returend value is expected to be equal to expectedResult", DEFAULT_ADDR, localIfc.getLocalIfcAddr());
}

CPPUNIT_TEST_SUITE_REGISTRATION(ILocalIfcTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
