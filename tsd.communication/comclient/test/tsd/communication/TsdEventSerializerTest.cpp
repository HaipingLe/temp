////////////////////////////////////////////////////////////////////////////////
///  @file TsdEventSerializerTest.cpp
///  @brief Test implementation for TsdEventSerializer
///  @author user@technisat.de
///  Copyright (c) TechniSat Digital GmbH,
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <sys/time.h>

#ifdef LOG_DISABLED
#define LOG(message)
#else
#define LOG_CLASS "TsdEventSerializer"
#ifdef LOG_LONG_FORMAT
#define LOG_CLASS __FILE__
#endif
#define LOG(message) Log(LOG_CLASS, __LINE__, LOG_CLASS, __func__, message);
#endif

#define LOG_ENTER LOG("Entered");
#define LOG_EXIT LOG("Exited");

#ifndef LOG_DISABLED
static void Log(const char* file, unsigned int lineNumber,
		const char* className, const char* functionName, std::string message) {
	time_t now;
	struct tm *current;
	now = time(0);
	current = localtime(&now);
	struct timeval detail_time;
	gettimeofday(&detail_time, NULL);

	::std::cout << ::std::endl << 1900 + current->tm_year << "-"
			<< ::std::setw(2) << ::std::setfill('0') << 1 + current->tm_mon
			<< "-" << ::std::setw(2) << ::std::setfill('0') << current->tm_mday
			<< " " << ::std::setw(2) << ::std::setfill('0') << current->tm_hour
			<< ":" << ::std::setw(2) << ::std::setfill('0') << current->tm_min
			<< ":" << ::std::setw(2) << ::std::setfill('0') << current->tm_sec
			<< "." << detail_time.tv_usec / 1000 << " - " << className << "::"
			<< functionName << " [" << file << ":" << lineNumber << "]"
			<< ::std::endl << message << ::std::endl << ::std::flush;
}
#endif

#include "MallocHelper.h"

#include <tsd/communication/IEventSerializer.hpp>

//the unit test header
#include "TsdEventSerializerTest.hpp"

using namespace std;
using namespace tsd::communication;
using namespace tsd::communication::event;
using namespace tsd::common::ipc;

namespace tsd {
namespace communication {

class IEventSerializer;

class Serializer: public IEventSerializer {
public:
	Serializer(uint32_t id) {
		m_id = id;
		m_deserializeCalledCount = 0;
		m_serializeCalledCount = 0;
	}

	virtual ~Serializer() {
	}

	std::auto_ptr<TsdEvent> deserialize(RpcBuffer& buffer) {
		uint32_t id = buffer.getInt();
		std::auto_ptr<TsdEvent> result(new TsdEvent(id));

		m_deserializeCalledCount++;

		return result;
	}

	void serialize(common::ipc::RpcBuffer& /* buf */,
			const event::TsdEvent& obj) {

		CPPUNIT_ASSERT_EQUAL(m_id, obj.getEventId());

		m_serializeCalledCount++;
	}

	uint32_t m_id;
	int m_serializeCalledCount;
	int m_deserializeCalledCount;
};

/**
 * Creates a new \link TsdEventSerializerTest \linkend.
 */
TsdEventSerializerTest::TsdEventSerializerTest() {
	/* empty */
}

/**
 * Initializes the unit test.
 */
void TsdEventSerializerTest::setUp() {
	/* empty */
}

/**
 * Clean up unit test.
 */
void TsdEventSerializerTest::tearDown() {
	/* empty */
}

/**
 * Test implementation of \link TsdEventSerializer::getInstance \endlink.
 * It is verified that:<br>
 * <ul>
 * 	<li>On multiple calls no memory is de-/allocated</li>
 * 	<li>On multiple calls exactly the same TsdEventSerializer object is returned</li>
 * </ul>
 */
void TsdEventSerializerTest::test_getInstance() {
	LOG_ENTER

	resetMemoryStatistics();

	TsdEventSerializer* dut = TsdEventSerializer::getInstance();

	setCapturingMemoryStatisticsEnabled(false);

	size_t temp_mallocCount = mallocCount;
	size_t temp_freeCount = freeCount;

	CPPUNIT_ASSERT(dut != NULL);

	setCapturingMemoryStatisticsEnabled(true);

	TsdEventSerializer* dut2 = TsdEventSerializer::getInstance();

	setCapturingMemoryStatisticsEnabled(false);

	CPPUNIT_ASSERT(dut == dut2);
	CPPUNIT_ASSERT_EQUAL(temp_mallocCount, mallocCount);
	CPPUNIT_ASSERT_EQUAL(temp_freeCount, freeCount);

	printMallocDiagnostics("TsdEventSerializerTest::test_getInstance:");

	LOG_EXIT
}

/**
 * Test implementation of \link TsdEventSerializer::serialize \endlink.
 * It is verified that:<br>
 * <ul>
 * 	<li>An event is serialized properly.</li>
 * </ul>
 */
void TsdEventSerializerTest::test_serialize() {
	LOG_ENTER

	setCapturingMemoryStatisticsEnabled(false);

	RpcBuffer sendBuffer;
	vector<char> data(10);
	sendBuffer.init(&data);

	TsdEventSerializer* dut = TsdEventSerializer::getInstance();

	CPPUNIT_ASSERT(dut != NULL);
	dut->serialize(sendBuffer, TsdEvent(0xDEADFACE));

	RpcBuffer receiveBuffer;
	receiveBuffer.init(&data);
	CPPUNIT_ASSERT_EQUAL(0xDEADFACE, (uint32_t) receiveBuffer.peekInt());

	LOG_EXIT
}

/**
 * Test implementation of \link TsdEventSerializer::addSerializer \endlink, \link
 * TsdEventSerializer::deserialize \endlink and \link TsdEventSerializer::removeSerializer \endlink.
 * It is verified that:<br>
 * <ul>
 * 	<li>adding an empty list of event ids has no effect</li>
 * 	<li>adding 0 ids via the array based method has no effect</li>
 * 	<li>calling deserializer on an empty event id list will have no effect</li>
 * 	<li>deserialize properly reconstructs an event stored within a buffer.</li>
 * 	<li>that the correct serializers are called.</li>
 * 	<li>that deserializing an event for which no serializer exists will not affect the others.</li>
 * </ul>
 */
void TsdEventSerializerTest::test_deserialize() {
	LOG_ENTER

	setCapturingMemoryStatisticsEnabled(false);

	vector<Serializer*> serializer;
	RpcBuffer buffer;
	char data[] = { (char) 0xCE, (char) 0xFA, (char) 0xAD, (char) 0xDE, 0x78,
			0x56, 0x34, 0x12 };
	buffer.init(data, sizeof(data));
	size_t remainingReadSize = buffer.getRemainingReadSize();
	vector<uint32_t> eventIds;

	/* special Serializer */
	serializer.push_back(new Serializer(0));

	/* for each event one Serializer */
	for (uint32_t i = 1; i <= 10; i++) {
		serializer.push_back(new Serializer(i));
	}

	TsdEventSerializer* dut = TsdEventSerializer::getInstance();

	CPPUNIT_ASSERT(dut != NULL);

	/* verify adding an empty list of Ids and calling deserialize will not
	 * call the serializer
	 */
	dut->addSerializer(serializer[0], eventIds);

	TsdEvent* event = dut->deserialize(buffer).get();

	/* the buffer should not have been modified */
	CPPUNIT_ASSERT(event == NULL);
	CPPUNIT_ASSERT_EQUAL(remainingReadSize, buffer.getRemainingReadSize());

	uint32_t temp[] = { 0xDEADFACE };
	dut->addSerializer(serializer[0], temp, 0);

	event = dut->deserialize(buffer).get();

	/* the buffer should not have been modified */
	CPPUNIT_ASSERT(event == NULL);
	CPPUNIT_ASSERT_EQUAL(remainingReadSize, buffer.getRemainingReadSize());

	/*
	 * Verify deserializing a 32 bit event ID.
	 */
	dut->addSerializer(serializer[0], temp, 1);
	std::auto_ptr<TsdEvent> pEvent = dut->deserialize(buffer);
	event = pEvent.get();

	CPPUNIT_ASSERT(event != NULL);
	CPPUNIT_ASSERT_EQUAL(1, serializer[0]->m_deserializeCalledCount);
	CPPUNIT_ASSERT_EQUAL(remainingReadSize - 4, buffer.getRemainingReadSize());
	CPPUNIT_ASSERT_EQUAL(0xDEADFACE, event->getEventId());

	/*
	 * Verify that the correct serializers are called.
	 */
	/* Add ids via array based method */
	for (uint32_t i = 1; i <= 5; i++) {
		eventIds.clear(); /* only add one Serializer */
		eventIds.push_back(i);
		dut->addSerializer(serializer[i], &eventIds[0], 1);
	}

	/* Adding ids via vector based method */
	for (uint32_t i = 6; i <= 10; i++) {
		eventIds.clear(); /* only add one Serializer */
		eventIds.push_back(i);
		dut->addSerializer(serializer[i], eventIds);
	}

	for (uint32_t i = 1; i <= 10; i++) {
		RpcBuffer buffer;
		char data[] = { (char) ((i & 0x000000FF)), (char) 0, (char) 0, (char) 0,
				0x78, 0x56, 0x34, 0x12 };
		buffer.init(data, sizeof(data));
		size_t remainingReadSize = buffer.getRemainingReadSize();

		std::auto_ptr < TsdEvent > pEvent = dut->deserialize(buffer);
		event = pEvent.get();

		CPPUNIT_ASSERT(event != NULL);
		CPPUNIT_ASSERT_EQUAL(1, serializer[i]->m_deserializeCalledCount);
		CPPUNIT_ASSERT_EQUAL(remainingReadSize - 4,
				buffer.getRemainingReadSize());
		CPPUNIT_ASSERT_EQUAL(i, event->getEventId());

		pEvent.reset(NULL);
	}

	/* verify deserializing an event for which no serializer is available */
	RpcBuffer buffer2;
	char data2[] = { (char) 0xAD, (char) 0xDE, (char) 0xFE, (char) 0xAF, 0x78,
			0x56, 0x34, 0x12 };
	buffer2.init(data2, sizeof(data2));
	remainingReadSize = buffer2.getRemainingReadSize();
	pEvent = dut->deserialize(buffer2);
	event = pEvent.get();
	CPPUNIT_ASSERT(event == NULL);
	CPPUNIT_ASSERT_EQUAL(remainingReadSize, buffer2.getRemainingReadSize());


	for (int i = 0; i < (int)serializer.size(); i++) {
		/* verify that no serializer was called */
		CPPUNIT_ASSERT_EQUAL(1, serializer[i]->m_deserializeCalledCount);

		dut->removeSerializer(serializer[i]);

		delete serializer[i];
	}

	serializer.clear();

	LOG_EXIT
}

/**
 * Test implementation of \link TsdEventSerializer::removeSerializer \endlink.
 * It is verified that:<br>
 * <ul>
 * 	<li>when a serializer is removed it will not receive any events even when it was registered for multiple events.</li>
 * 	<li>when a serializer is removed will still receive the events.</li>
 * </ul>
 */
void TsdEventSerializerTest::test_removeSerializer() {
	LOG_ENTER

	setCapturingMemoryStatisticsEnabled(false);

	vector<Serializer*> serializersToBeRemoved;
	vector<Serializer*> serializersNotRemoved;
	RpcBuffer buffer;
	const uint32_t maxNumSerializers = 3;

	TsdEventSerializer* dut = TsdEventSerializer::getInstance();

	CPPUNIT_ASSERT(dut != NULL);

	/* just to fill the gap because serializer IDs will start at 1 */
	serializersToBeRemoved.push_back(new Serializer(0));
	serializersNotRemoved.push_back(new Serializer(0));

	/* One Serializer for a sequence of event IDs */
	for (uint32_t serializerID = 1; serializerID <= maxNumSerializers; serializerID++) {
		vector<uint32_t> eventIds;

		/* generate maxNumEvents subsequent event IDs for each serializer */
		uint32_t startID = (((serializerID - 1) * maxNumSerializers) + 1);
		uint32_t endID = startID + maxNumSerializers - 1;
		for (uint32_t eventID = startID; eventID <= endID; eventID++) {
			eventIds.push_back(eventID);
		}

		serializersToBeRemoved.push_back(new Serializer(serializerID));
		serializersNotRemoved.push_back(new Serializer(serializerID));

		dut->addSerializer(serializersToBeRemoved[serializerID], eventIds);
		dut->addSerializer(serializersNotRemoved[serializerID], eventIds);
	}

	/* verify that after removing the serializer it is not called any longer */
	for (uint32_t serializerID = 1; serializerID <= maxNumSerializers; serializerID++) {

		uint32_t startID = (((serializerID - 1) * maxNumSerializers) + 1);
		uint32_t endID = startID + maxNumSerializers - 1;
		for (uint32_t eventID = startID, count = 1; eventID <= endID; eventID++, count++) {
			char data[] = { (char) ((eventID & 0x000000FF)), (char) 0, (char) 0, (char) 0, 0x78, 0x56, 0x34, 0x12 };
			RpcBuffer buffer;
			buffer.init(data, sizeof(data));

			size_t remainingReadSize = buffer.getRemainingReadSize();

			/* remove one serializer but not the other one */
			dut->removeSerializer(serializersToBeRemoved[serializerID]);

			std::auto_ptr<TsdEvent> pEvent = dut->deserialize(buffer);
			TsdEvent* event = pEvent.get();

			CPPUNIT_ASSERT(event != NULL);
			CPPUNIT_ASSERT_EQUAL(0,	serializersToBeRemoved[serializerID]->m_deserializeCalledCount);
			CPPUNIT_ASSERT_EQUAL(count,	(uint32_t)serializersNotRemoved[serializerID]->m_deserializeCalledCount);
			CPPUNIT_ASSERT_EQUAL(remainingReadSize - 4,	buffer.getRemainingReadSize());

			pEvent.reset(NULL);
		}
	}

	for(int i = 1; i < (int)serializersToBeRemoved.size(); i++){
		/* verify that no removed serializer was called */
		CPPUNIT_ASSERT_EQUAL(0,	serializersToBeRemoved[i]->m_deserializeCalledCount);

		/* verify that no serializer not removed were called but only once */
		CPPUNIT_ASSERT_EQUAL(maxNumSerializers,	(uint32_t)serializersNotRemoved[i]->m_deserializeCalledCount);

		delete serializersToBeRemoved[i];
		delete serializersNotRemoved[i];
	}

	serializersToBeRemoved.clear();
	serializersNotRemoved.clear();

	LOG_EXIT
}

CPPUNIT_TEST_SUITE_REGISTRATION (TsdEventSerializerTest);

} // - namespace tsd
} // - namespace communication
