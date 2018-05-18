//////////////////////////////////////////////////////////////////////
/// @file AddressInUseExceptionTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test AddressInUseException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "AddressInUseExceptionTest.hpp"
#include <tsd/communication/messaging/AddressInUseException.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
std::string DEFAULT_MESSAGE = "AddressInUseException";
}

void AddressInUseExceptionTest::test_Constructor_WithMsg_ObjectCreated()
{
   std::shared_ptr<AddressInUseException> addressInUseException;
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw",
                                   addressInUseException = std::make_shared<AddressInUseException>(DEFAULT_MESSAGE));
   CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Returend value is expected to be equal to default name", DEFAULT_MESSAGE, (std::string)addressInUseException->what());
}

CPPUNIT_TEST_SUITE_REGISTRATION(AddressInUseExceptionTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
