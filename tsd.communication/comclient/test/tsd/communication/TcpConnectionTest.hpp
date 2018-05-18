////////////////////////////////////////////////////////////////////////////////
///  @file TcpConnectionTest.hpp
///  @brief Test for class Connection
///  @author user@technisat.de
///  Copyright (c) TechniSat Digital GmbH, 2014
////////////////////////////////////////////////////////////////////////////////

#ifndef TcpConnectionTest_HPP_
#define TcpConnectionTest_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <tsd/common/types/typedef.hpp>

#include <tsd/communication/Connection.hpp>

namespace tsd {
namespace communication {

////////////////////////////////////////////////////////////////////////////////
///  @brief Test suite for the class Connection
////////////////////////////////////////////////////////////////////////////////
class TcpConnectionTest: public CPPUNIT_NS::TestFixture
{
   public:
      TcpConnectionTest();
      void setUp();
      void tearDown();

      void test_Constructor();
      void test_send_SingleFrames();
      void test_send_Burst();

   private:
      CPPUNIT_TEST_SUITE(TcpConnectionTest);

      CPPUNIT_TEST(test_Constructor);
      CPPUNIT_TEST(test_send_SingleFrames);
      CPPUNIT_TEST(test_send_Burst);

      CPPUNIT_TEST_SUITE_END();
};

} // - namespace tsd
} // - namespace communication

#endif //TcpConnectionTest_HPP_
