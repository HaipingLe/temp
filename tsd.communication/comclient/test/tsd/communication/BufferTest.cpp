////////////////////////////////////////////////////////////////////////////////
///  @file BufferTest.cpp
///  @brief Test implementation for Buffer
///  @author user@technisat.de
///  Copyright (c) TechniSat Digital GmbH,
////////////////////////////////////////////////////////////////////////////////

/* switch off optimization to prevent that statements are re-ordered. */
#pragma GCC push_options
#pragma GCC optimize ("O0")

#include <iostream>
#include <iomanip>
#include <sys/time.h>

#ifdef LOG_DISABLED
	#define LOG(message)
#else
	#define LOG_CLASS "Buffer"
	#ifdef LOG_LONG_FORMAT
		#define LOG_CLASS __FILE__
	#endif
	#define LOG(message) Log(LOG_CLASS, __LINE__, LOG_CLASS, __func__, message);
#endif

#define LOG_ENTER LOG("Entered");
#define LOG_EXIT LOG("Exited");

#ifndef LOG_DISABLED
static void Log(const char* file, unsigned int lineNumber, const char* className, const char* functionName, std::string message){
	time_t now;
	struct tm *current;
	now = time(0);
	current = localtime(&now);
	struct timeval detail_time;
	gettimeofday(&detail_time,NULL);

	::std::cout << ::std::endl
		<< 1900 + current->tm_year
		<< "-" << ::std::setw(2) << ::std::setfill('0') << 1 + current->tm_mon
		<< "-" << ::std::setw(2) << ::std::setfill('0') << current->tm_mday
		<< " " << ::std::setw(2) << ::std::setfill('0') << current->tm_hour
		<< ":" << ::std::setw(2) << ::std::setfill('0') << current->tm_min
		<< ":" << ::std::setw(2) << ::std::setfill('0') << current->tm_sec
		<< "." << detail_time.tv_usec/1000
		<< " - " << className << "::" << functionName << " [" << file << ":" << lineNumber << "]" << ::std::endl
		<< message << ::std::endl << ::std::flush;
}
#endif

#include "MallocHelper.h"

//the unit test header
#include "BufferTest.hpp"

using namespace std;
using namespace tsd::communication;

namespace tsd {
namespace communication {

CPPUNIT_PARAMETERIZED_TEST_SUITE_REGISTRATION(BufferTest, BufferTest::TestVector);

static string buildDataMismatchMessage(size_t index, size_t expectedValue, size_t actualValue){
	ostringstream result;
	result << "Data mismatch at " << (int) index << ": "
			<< static_cast<unsigned>(expectedValue) << " == " << static_cast<unsigned>(actualValue);

	return result.str();
}

/**
 * Creates a new \link BufferTest \linkend.
 */
BufferTest::BufferTest()
{
    // empty
}

/**
 * Initializes the unit test.
 */
void BufferTest::setUp()
{
    // empty
}

/**
 * Clean up unit test.
 */
void BufferTest::tearDown()
{
    // empty
}

/**
 * Test implementation for \link Buffer::allocBuffer \endlink with size = 1 - 13.
 * It is verified that:<br>
 * <ul>
 * 	<li>calling Buffer::allocBuffer() does only allocate as much memory as expected</li>
 * 	<li>calling Buffer::length() returns the correct length</li>
 * 	<li>calling Buffer::deref() frees allocated memory when the referenece counter == 1</li>
 * </ul>
 */
void BufferTest::test_allocBuffer()
{
    LOG_ENTER

    for(uint32_t i = 1; i <= 13; i++){
    	resetMemoryStatistics();

		Buffer* buffer = allocBuffer(i);

		/*
		 * A Buffer contains an uint_32 array of size 1 (4 bytes) which stores
		 * the first element of the payload. Thus allocBuffer() needs to
		 * allocate 4 Bytes less memory.
		 */
		size_t size = sizeof(Buffer) - 4 + i;
		if (size % 4 != 0) {
			/*
			 *  Buffer aligns the allocated memory at a 4 byte boundary
			 */
			size += (4 - (size % 4));
		}

		setCapturingMemoryStatisticsEnabled(false);

		CPPUNIT_ASSERT_EQUAL(i, buffer->length());
		CPPUNIT_ASSERT_EQUAL_MESSAGE(buildDataMismatchMessage(i, size, allocatedMemory()), size, allocatedMemory());

		setCapturingMemoryStatisticsEnabled(true);

		buffer->deref();

		setCapturingMemoryStatisticsEnabled(false);

		CPPUNIT_ASSERT_EQUAL_MESSAGE(buildDataMismatchMessage(i, 0, allocatedMemory()), 0, (int)trueAllocatedMemory());
    }

    LOG_EXIT
}

/**
 * Test implementation for \link Buffer::ref \endlink and \link Buffer::deref \endlink.
 * It is verified that:<br>
 * <ul>
 * 	<li>calling Buffer::ref() does not affect allocated memory</li>
 * 	<li>calling Buffer::deref() does not affect allocated memory since the reference counter is > 1</li>
 * 	<li>calling Buffer::deref() frees allocated memory when the referenece counter == 1</li>
 * </ul>
 */
void BufferTest::test_RefDeref()
{
    LOG_ENTER

    resetMemoryStatistics();

    Buffer* buffer = allocBuffer(1);

    buffer->ref();

    setCapturingMemoryStatisticsEnabled(false);

    CPPUNIT_ASSERT_EQUAL(1, (int)mallocCount);
    CPPUNIT_ASSERT_EQUAL(0, (int)freeCount);

    setCapturingMemoryStatisticsEnabled(true);

    buffer->deref();

    setCapturingMemoryStatisticsEnabled(false);

    CPPUNIT_ASSERT_EQUAL(1, (int)mallocCount);
    CPPUNIT_ASSERT_EQUAL(0, (int)freeCount);

    setCapturingMemoryStatisticsEnabled(true);

	buffer->deref();

	setCapturingMemoryStatisticsEnabled(false);

	CPPUNIT_ASSERT_EQUAL(1, (int)mallocCount);
	CPPUNIT_ASSERT_EQUAL(1, (int)freeCount);

	CPPUNIT_ASSERT_EQUAL(0, (int)trueAllocatedMemory());

    LOG_EXIT
}

/**
 * Test implementation for \link Buffer::fill \endlink.
 * It is verified that:<br>
 * <ul>
 * 	<li>Calling Buffer::fill multiple times to fill the Buffers works correctly</li>
 * </ul>
 */
void BufferTest::test_fill()
{
    LOG_ENTER



    uint32_t bufferSize = 1024;
    uint8_t* data = (uint8_t*) malloc(bufferSize);

    resetMemoryStatistics();

    Buffer* buffer = allocBuffer(bufferSize);


	/* generate test data */
	for (uint32_t i = 0; i < bufferSize; i++) {
		data[i] = (uint8_t) (1 << (i % 8));
	}

	uint32_t eventId = (uint32_t)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]);

	/* Fill the buffer incrementally and verify its state */
	uint32_t increment = bufferSize / 3;
    for(uint32_t i = 0; i < bufferSize; i += increment){
    	setCapturingMemoryStatisticsEnabled(true);

    	buffer->fill(&data[i], increment);

    	setCapturingMemoryStatisticsEnabled(false);

		CPPUNIT_ASSERT_EQUAL((i + increment < bufferSize) ? false : true, buffer->isFilled());
		CPPUNIT_ASSERT_EQUAL(eventId, buffer->eventId());
		CPPUNIT_ASSERT_EQUAL(bufferSize, buffer->length());
		CPPUNIT_ASSERT_EQUAL(bufferSize + 4, buffer->lengthWithHeader());

		/*
		 * length is encoded within the first 4 bytes
		 */
		void* payloadWithHeader = buffer->payloadWithHeader();
		CPPUNIT_ASSERT_EQUAL(((uint32_t*) payloadWithHeader)[0], buffer->length());

		/*
		 * payload starts after the first 4 bytes
		 */
		uint8_t* payload = (uint8_t*) buffer->payload();
		CPPUNIT_ASSERT_EQUAL(&((uint8_t*) payloadWithHeader)[4], payload);

		/* verify payload */
		for (uint32_t p = 0; p < i ; p++) {
			CPPUNIT_ASSERT_EQUAL_MESSAGE(
					buildDataMismatchMessage(p, data[p], payload[p]), data[p],
					payload[p]);
		}
	}

    setCapturingMemoryStatisticsEnabled(true);

	buffer->deref();

	setCapturingMemoryStatisticsEnabled(false);

	CPPUNIT_ASSERT_EQUAL(1, (int)mallocCount);
	CPPUNIT_ASSERT_EQUAL(1, (int)freeCount);

	CPPUNIT_ASSERT_EQUAL(0, (int)trueAllocatedMemory());

	free(data);

    LOG_EXIT
}

/**
 * Provides a list of TestVectors. The TestVectors contai test data to test:
 * <ul>
 * 	<li>Buffer sizes from 1 - 13 byte. These data is mainly intended to test the behaviour of Buffer at 4 byte boundaries.</li>
 * 	<li>Behaviour of Buffer at boundary: 16K - 1, 16K, 16K + 1</li>
 * 	<li>Behaviour of Buffer at boundary: 32K - 1, 32K, 32K + 1</li>
 * 	<li>Behaviour of Buffer at boundary: 64K - 1, 64K, 64K + 1</li>
 * 	<li>Behaviour of Buffer at boundary: 128K - 1, 128K, 128K + 1</li>
 * 	<li>Behaviour of Buffer at boundary: 512K - 1, 512K, 512K + 1</li>
 * 	<li>Behaviour of Buffer at boundary: 1M - 1, 1M, 1M + 1</li>
 * </ul>
 */
std::vector<BufferTest::TestVector> BufferTest::parameters() {
	std::vector<BufferTest::TestVector> result;

	for(uint32_t i = 1; i <= 13; i++){
		result.push_back(TestVector(1, i));
	}

	/* Test vectors for 16K boundary (16K - 1, 16K, 16K + 1)*/
	uint32_t size = (16 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	/* Test vectors for 32K boundary (32K - 1, 32K, 32K + 1)*/
	size = (32 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	/* Test vectors for 64K boundary (64K - 1, 64K, 64K + 1)*/
	size = (64 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	/* Test vectors for 128K boundary (128K - 1, 128K, 128K + 1)*/
	size = (128 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	/* Test vectors for 256K boundary (256K - 1, 256K, 256K + 1)*/
	size = (256 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	/* Test vectors for 512K boundary (512K - 1, 512K, 512K + 1)*/
	size = (512 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	/* Test vectors for 1M boundary (1M - 1, 1M, 1M + 1)*/
	size = (1024 * 1024) - 1;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	size++;
	result.push_back(TestVector(size, size));

	return result;
}

/**
 * Test implementation for \link Buffer::fill \endlink.
 * The following methods are verified:<br>
 * <ul>
 * 	<li>Buffer::isFilled</li>
 * 	<li>Buffer::eventId</li>
 * 	<li>Buffer::length</li>
 * 	<li>Buffer::lengthWithHeader</li>
 * 	<li>Buffer::payloadWithHeader</li>
 * 	<li>Buffer::payload</li>
 * 	<li>Buffer::fill</li>
 * </ul><br>
 * <br>
 * The test uses two parameters TestVector::dataSize which determins the initial
 * size of the test data and TestVector::bufferSize which specifies the the size
 * of the Buffer. The test is repeated with incremented dataSize until
 * dataSize == bufferSize + 1. This way it is verified that less data then the
 * bufferSize can be written to a Buffer as well as a maximum of bufferSize is
 * written even if the dataSize is greater.
 */
void BufferTest::testWithParameter(BufferTest::TestVector testVector) {

	ostringstream message;
	message << "TestVector: dataSize=" << testVector.dataSize << ", bufferSize=" << testVector.bufferSize;
	LOG(message.str());

	uint32_t bufferSize = testVector.bufferSize;

	/*
	 * Verify that fill() respects of Buffer. Thus provide a length which
	 * is greater than that of Buffer.
	 */
	for (uint32_t dataSize = testVector.dataSize; dataSize <= bufferSize + 1; dataSize++) {

		size_t size = dataSize;
		if (size % 4 != 0) {
			/*
			 *  Buffer aligns the allocated memory at a 4 byte boundary
			 */
			size += (4 - (size % 4));
		}

		uint8_t* data = (uint8_t*) malloc(size);

		/* generate test data */
		for (uint32_t i = 0; i < size; i++) {
			data[i] = (uint8_t) (1 << (i % 8));
		}

		resetMemoryStatistics();

		Buffer* buffer = allocBuffer(bufferSize);

		buffer->fill(data, dataSize);

		setCapturingMemoryStatisticsEnabled(false);

		CPPUNIT_ASSERT_EQUAL((dataSize < bufferSize) ? false : true,
				buffer->isFilled());

		/*
		 * Buffer stores the eventId in the first 4 bytes of the payload,
		 * which are always present even when the Buffer was allocated with
		 * a value less the 4. Thus when the Buffer's is less than 4 bytes
		 * the unused bytes contains random data and must be masked when
		 * verified.
		 */
		uint32_t eventIdMask = 0xFF;
		uint32_t eventId = data[0];
		uint32_t numEventIdBytes =
				(dataSize < bufferSize) ? dataSize : bufferSize;
		if (numEventIdBytes > 1) {
			eventIdMask |= 0xFF << 8;
			eventId |= data[1] << 8;
		}
		if (numEventIdBytes > 2) {
			eventIdMask |= 0xFF << 16;
			eventId |= data[2] << 16;
		}
		if (numEventIdBytes > 3) {
			eventIdMask |= 0xFF << 24;
			eventId |= data[3] << 24;
		}

		CPPUNIT_ASSERT_EQUAL(eventId, buffer->eventId() & eventIdMask);
		CPPUNIT_ASSERT_EQUAL(bufferSize, buffer->length());
		CPPUNIT_ASSERT_EQUAL(bufferSize + 4, buffer->lengthWithHeader());

		/*
		 * length is encoded within the first 4 bytes
		 */
		void* payloadWithHeader = buffer->payloadWithHeader();
		CPPUNIT_ASSERT_EQUAL(((uint32_t*) payloadWithHeader)[0],
				buffer->length());

		/*
		 * payload starts after the first 4 bytes
		 */
		uint8_t* payload = (uint8_t*) buffer->payload();
		CPPUNIT_ASSERT_EQUAL(&((uint8_t*) payloadWithHeader)[4], payload);

		/* verify payload */
		for (uint32_t i = 0; i < dataSize && dataSize < bufferSize; i++) {
			CPPUNIT_ASSERT_EQUAL_MESSAGE(
					buildDataMismatchMessage(i, data[i], payload[i]), data[i],
					payload[i]);
		}

		setCapturingMemoryStatisticsEnabled(true);

		buffer->deref();

		setCapturingMemoryStatisticsEnabled(false);

		CPPUNIT_ASSERT_EQUAL_MESSAGE(
				buildDataMismatchMessage(bufferSize, 0, allocatedMemory()), 0,
				(int) trueAllocatedMemory());

		free(data);
	}
}
} // - namespace tsd
} // - namespace communication

#pragma GCC pop_options
