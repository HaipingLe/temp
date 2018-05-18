////////////////////////////////////////////////////////////////////////////////
///  @file ConnectionTest.hpp
///  @brief Test for class Connection
///  @author user@technisat.de
///  Copyright (c) TechniSat Digital GmbH, 2014
////////////////////////////////////////////////////////////////////////////////

#ifndef ConnectionTest_HPP_
#define ConnectionTest_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <tsd/common/types/typedef.hpp>

#include <tsd/communication/Connection.hpp>

namespace tsd {
namespace communication {

////////////////////////////////////////////////////////////////////////////////
///  @brief Test suite for the class Connection
////////////////////////////////////////////////////////////////////////////////
class ConnectionTest: public CPPUNIT_NS::TestFixture
{
   public:
      ConnectionTest();
      void setUp();
      void tearDown();

      void test_openConnection();

   private:
      CPPUNIT_TEST_SUITE(ConnectionTest);

      CPPUNIT_TEST(test_openConnection);

      CPPUNIT_TEST_SUITE_END();
};

} // - namespace tsd
} // - namespace communication

#endif //ConnectionTest_HPP_
