//////////////////////////////////////////////////////////////////////
/// @file ConnectionExceptionTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test ConnectionException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTIONEXCEPTIONTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTIONEXCEPTIONTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for ConnectionException
 *
 * @brief Testclass for ConnectionException
 */
class ConnectionExceptionTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with msg
    *
    * @tsd_testobject tsd::communication::messaging::ConnectionException::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithMsg_ObjectCreated();

   CPPUNIT_TEST_SUITE(ConnectionExceptionTest);
   CPPUNIT_TEST(test_Constructor_WithMsg_ObjectCreated);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_CONNECTIONEXCEPTIONTEST_HPP
