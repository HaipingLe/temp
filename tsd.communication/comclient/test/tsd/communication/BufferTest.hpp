////////////////////////////////////////////////////////////////////////////////
///  @file BufferTest.hpp
///  @brief Test for class Buffer
///  @author user@technisat.de
///  Copyright (c) TechniSat Digital GmbH, 2014
////////////////////////////////////////////////////////////////////////////////

#ifndef BufferTest_HPP_
#define BufferTest_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <tsd/common/types/typedef.hpp>
#include <cppunit/extensions/Parameterized.hpp>

#include <tsd/communication/Buffer.hpp>

namespace tsd {
namespace communication {

////////////////////////////////////////////////////////////////////////////////
///  @brief Test suite for the class Buffer
////////////////////////////////////////////////////////////////////////////////
class BufferTest: public CPPUNIT_NS::TestFixture
{
   private:
	class TestVector{
		public:
			uint32_t dataSize;
			uint32_t bufferSize;

			TestVector(uint32_t dataSize, uint32_t bufferSize)
			:dataSize(dataSize),
			 bufferSize(bufferSize){
				// empty
			}
	};

   public:
      BufferTest();
      void setUp();
      void tearDown();
      void test_allocBuffer();
      void test_RefDeref();
      void test_fill();

	/**
	 * Retrieves the test parameters. Each entry is passed as a single parameter
	 * to a test.
	 *
	 * @return the parameters list.
	 */
	static std::vector<BufferTest::TestVector> parameters();

	/**
	 * This method is called with a single test parameter.
	 *
	 * @param parameter The parameter to be used for the test.
	 */
	void testWithParameter(BufferTest::TestVector parameter);

   private:
      CPPUNIT_PARAMETERIZED_TEST_SUITE(BufferTest, BufferTest::TestVector);

      CPPUNIT_TEST(test_allocBuffer);
      CPPUNIT_TEST(test_RefDeref);
      CPPUNIT_TEST(test_fill);

      CPPUNIT_PARAMETERIZED_TEST_SUITE_END();
};

} // - namespace tsd
} // - namespace communication

#endif //BufferTest_HPP_
