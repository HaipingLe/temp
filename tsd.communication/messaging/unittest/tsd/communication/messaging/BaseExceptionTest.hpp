//////////////////////////////////////////////////////////////////////
/// @file BaseExceptionTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test BaseException
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_BASEEXCEPTIONTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_BASEEXCEPTIONTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for BaseException
 *
 * @brief Testclass for BaseException
 */
class BaseExceptionTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: with msg
    *
    * @tsd_testobject tsd::communication::messaging::BaseException::Constructor
    * @tsd_testexpected object created
    */
   void test_Constructor_WithMsg_ObjectCreated();
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::BaseException::What
    * @tsd_testexpected member what c str returned
    */
   void test_What_JustRun_MemberWhatCStrReturned();

   CPPUNIT_TEST_SUITE(BaseExceptionTest);
   CPPUNIT_TEST(test_Constructor_WithMsg_ObjectCreated);
   CPPUNIT_TEST(test_What_JustRun_MemberWhatCStrReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_BASEEXCEPTIONTEST_HPP
