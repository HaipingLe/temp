//////////////////////////////////////////////////////////////////////
/// @file BaseExceptionTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test BaseException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "BaseExceptionTest.hpp"
#include <tsd/communication/messaging/BaseException.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
std::string DEFAULT_MESSAGE = "BaseException";
}

void BaseExceptionTest::test_Constructor_WithMsg_ObjectCreated()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", std::make_shared<BaseException>(DEFAULT_MESSAGE));
}

void BaseExceptionTest::test_What_JustRun_MemberWhatCStrReturned()
{
   BaseException testObj(DEFAULT_MESSAGE);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("Returend value is expected to be equal to default name", DEFAULT_MESSAGE, (std::string)testObj.what());
}

CPPUNIT_TEST_SUITE_REGISTRATION(BaseExceptionTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
