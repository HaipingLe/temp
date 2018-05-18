//////////////////////////////////////////////////////////////////////
/// @file InvalidArgumentExceptionTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test InvalidArgumentException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "InvalidArgumentExceptionTest.hpp"
#include <tsd/communication/messaging/InvalidArgumentException.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
std::string DEFAULT_MESSAGE = "Invalid Argument Exception";
}

void InvalidArgumentExceptionTest::test_Constructor_WithMsg_ObjectCreated()
{
   std::shared_ptr<InvalidArgumentException> invalidArgumentException;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw",
                                   invalidArgumentException = std::make_shared<InvalidArgumentException>(DEFAULT_MESSAGE));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Returend value is expected to be equal to default name", DEFAULT_MESSAGE, (std::string)invalidArgumentException->what());
}

CPPUNIT_TEST_SUITE_REGISTRATION(InvalidArgumentExceptionTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
