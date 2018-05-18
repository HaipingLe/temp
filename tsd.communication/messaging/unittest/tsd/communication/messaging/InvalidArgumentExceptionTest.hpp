//////////////////////////////////////////////////////////////////////
/// @file InvalidArgumentExceptionTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test InvalidArgumentException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_INVALIDARGUMENTEXCEPTIONTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_INVALIDARGUMENTEXCEPTIONTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for InvalidArgumentException
 *
 * @brief Testclass for InvalidArgumentException
 */
class InvalidArgumentExceptionTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with msg
    *
    * @tsd_testobject tsd::communication::messaging::InvalidArgumentException::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithMsg_ObjectCreated();

   CPPUNIT_TEST_SUITE(InvalidArgumentExceptionTest);
   CPPUNIT_TEST(test_Constructor_WithMsg_ObjectCreated);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_INVALIDARGUMENTEXCEPTIONTEST_HPP
