//////////////////////////////////////////////////////////////////////
/// @file IRemoteIfcTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test IRemoteIfc
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "IRemoteIfcTest.hpp"
#include <tsd/communication/messaging/IRemoteIfc.hpp>
#include <tsd/communication/messaging/IRemoteIfcMock.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
tsd::communication::event::IfcAddr_t DEFAULT_LOCAL_ADDR{1};
tsd::communication::event::IfcAddr_t DEFAULT_REMOTE_ADDR{2};
}

void IRemoteIfcTest::test_Constructor_WithLocalIfcAddrAndRemoteIfcAddr_ObjectCreated()
{
   IRemoteIfc* remoteIfc;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", remoteIfc = new IRemoteIfcMock(DEFAULT_LOCAL_ADDR, DEFAULT_REMOTE_ADDR));
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Destructor should not throw", delete remoteIfc);
}

void IRemoteIfcTest::test_GetLocalIfcAddr_JustRun_MemberLocalIfcAddrReturned()
{
   IRemoteIfcMock testObj(DEFAULT_LOCAL_ADDR, DEFAULT_REMOTE_ADDR);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returend value is expected to be equal to expectedResult", DEFAULT_LOCAL_ADDR, testObj.getLocalIfcAddr());
}

void IRemoteIfcTest::test_GetRemoteIfcAddr_JustRun_MemberRemoteIfcAddrReturned()
{
   IRemoteIfcMock testObj(DEFAULT_LOCAL_ADDR, DEFAULT_REMOTE_ADDR);
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Returend value is expected to be equal to expectedResult", DEFAULT_REMOTE_ADDR, testObj.getRemoteIfcAddr());
}

CPPUNIT_TEST_SUITE_REGISTRATION(IRemoteIfcTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
