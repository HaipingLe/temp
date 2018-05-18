//////////////////////////////////////////////////////////////////////
/// @file ConnectionExceptionTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test ConnectionException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "ConnectionExceptionTest.hpp"
#include <tsd/communication/messaging/ConnectionException.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
std::string DEFAULT_MESSAGE = "Connection Exception";
}

void ConnectionExceptionTest::test_Constructor_WithMsg_ObjectCreated()
{
   std::shared_ptr<ConnectionException> connectionException;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw",
                                   connectionException = std::make_shared<ConnectionException>(DEFAULT_MESSAGE));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Returend value is expected to be equal to default name", DEFAULT_MESSAGE, (std::string)connectionException->what());
}

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionExceptionTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
