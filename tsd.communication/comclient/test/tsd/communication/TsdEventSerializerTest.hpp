////////////////////////////////////////////////////////////////////////////////
///  @file TsdEventSerializerTest.hpp
///  @brief Test for class TsdEventSerializer
///  @author user@technisat.de
///  Copyright (c) TechniSat Digital GmbH, 2014
////////////////////////////////////////////////////////////////////////////////

#ifndef TsdEventSerializerTest_HPP_
#define TsdEventSerializerTest_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <tsd/common/types/typedef.hpp>

#include <tsd/communication/TsdEventSerializer.hpp>

namespace tsd {
namespace communication {

////////////////////////////////////////////////////////////////////////////////
///  @brief Test suite for the class TsdEventSerializer
////////////////////////////////////////////////////////////////////////////////
class TsdEventSerializerTest: public CPPUNIT_NS::TestFixture
{
   public:
      TsdEventSerializerTest();
      void setUp();
      void tearDown();

      void test_getInstance();
      void test_serialize();
      void test_deserialize();
      void test_removeSerializer();
   private:
      CPPUNIT_TEST_SUITE(TsdEventSerializerTest);

      CPPUNIT_TEST(test_getInstance);
      CPPUNIT_TEST(test_serialize);
      CPPUNIT_TEST(test_deserialize);
      CPPUNIT_TEST(test_removeSerializer);

      CPPUNIT_TEST_SUITE_END();
};

} // - namespace tsd
} // - namespace communication

#endif //TsdEventSerializerTest_HPP_
