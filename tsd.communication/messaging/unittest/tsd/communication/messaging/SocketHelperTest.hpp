//////////////////////////////////////////////////////////////////////
/// @file SocketHelperTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test SocketHelper
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_SOCKETHELPERTEST_HPP
#define TSD_COMMUNICATION_MESSAGING_SOCKETHELPERTEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for GlobalSocketHelper
 *
 * @brief Testclass for GlobalSocketHelper
 */
class GlobalSocketHelperTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: path length is greater than sizeof minus1
    *
    * @tsd_testobject tsd::communication::messaging::GlobalSocketHelper::ParseUnixPath
    * @tsd_testexpected connection exception thrown
    */
   void test_ParseUnixPath_PathLengthIsGreaterThanSizeofMinus1_ConnectionExceptionThrown();
   /**
    * @brief Test scenario: first char of sun path is not as expected
    *
    * @tsd_testobject tsd::communication::messaging::GlobalSocketHelper::ParseUnixPath
    * @tsd_testexpected false returned
    */
   void test_ParseUnixPath_FirstCharOfSunPathIsNotAsExpected_FalseReturned();
   /**
    * @brief Test scenario: path longer than one char
    *
    * @tsd_testobject tsd::communication::messaging::GlobalSocketHelper::ParseUnixPath
    * @tsd_testexpected true returned
    */
   void test_ParseUnixPath_PathLongerThanOneChar_TrueReturned();
   /**
    * @brief Test scenario: path length is one
    *
    * @tsd_testobject tsd::communication::messaging::GlobalSocketHelper::ParseUnixPath
    * @tsd_testexpected true returned
    */
   void test_ParseUnixPath_PathLengthIsOne_TrueReturned();

   CPPUNIT_TEST_SUITE(GlobalSocketHelperTest);
   CPPUNIT_TEST(test_ParseUnixPath_PathLengthIsGreaterThanSizeofMinus1_ConnectionExceptionThrown);
   CPPUNIT_TEST(test_ParseUnixPath_FirstCharOfSunPathIsNotAsExpected_FalseReturned);
   CPPUNIT_TEST(test_ParseUnixPath_PathLongerThanOneChar_TrueReturned);
   CPPUNIT_TEST(test_ParseUnixPath_PathLengthIsOne_TrueReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_SOCKETHELPERTEST_HPP
