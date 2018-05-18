////////////////////////////////////////////////////////////////////////////////
///  @file TcpConnectionTest.cpp
///  @brief Test implementation for Connection
///  @author user@technisat.de
///  @todo repair this test whith USE_HIPPOMOCKS=1
///  Copyright (c) TechniSat Digital GmbH,
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <vector>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#ifdef LOG_DISABLED
	#define LOG(message)
#else
	#define LOG_CLASS "TcpConnection"
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

#include <tsd/common/logging/Logger.hpp>
#include <tsd/communication/Buffer.hpp>

// #define USE_HIPPOMOCKS

#ifdef USE_HIPPOMOCKS
#include <hippomocks.h>
#endif

#include <iostream>

#include "net/ServerSocket.hpp"
#include "net/ClientSocket.hpp"
#include "net/SocketException.hpp"
#include "net/EOFException.hpp"

//the unit test header
#include "TcpConnectionTest.hpp"

#include <tsd/common/errors/SystemException.hpp>
#include <tsd/common/errors/ConnectException.hpp>
#include <tsd/communication/Connection.hpp>
#include <tsd/communication/TcpConnection.hpp>

using namespace std;
using namespace tsd::common::types;
using namespace tsd::communication;
using namespace tsd::communication::client;
using namespace tsd::common::logging;

namespace tsd {
namespace communication {

typedef struct{
	ServerSocket* serverSocket;
	uint32_t numConnected;
	uint32_t numConnects;
	uint32_t numDisconnects;
	bool echoData;
	vector<string>* receivedData;
}ServerParameters;

#ifdef USE_HIPPOMOCKS
class LoggerMockup: public tsd::common::logging::Logger{
public:
	LoggerMockup(): Logger(""){};
	~LoggerMockup(){};
};
#else
class testReceiver: public client::IReceiveCallback
{
   public:
      void messageReceived(const void *buf, uint32_t len)
      {
         UNUSED(buf);
         UNUSED(len);
      };
      void disconnected() {};
};
#endif

static string toHex(const char* data, size_t size){
    ostringstream result;

    result << std::hex << std::setfill('0');

    for (size_t i = 0; i < size; i++) {
        result << std::setw(2) << static_cast<unsigned>(data[i] & 0xFF);
    }

    return result.str();
}

static string StringToHex(string& s){
	return toHex(s.c_str(), s.size());
}

static void* runServer(void* serverParameters){
	LOG_ENTER

	ServerParameters* parameters = (ServerParameters*)serverParameters;

	try {
		while (true) {
			ServerSocket socket;
			parameters->serverSocket->accept(socket);
			parameters->numConnected++;
			parameters->numConnects++;

			try {
				while (true) {
					string data;

					socket >> data;

					if(parameters->receivedData != NULL)
						parameters->receivedData->push_back(data);

					LOG(string("Received: ") + StringToHex(data));

					if(parameters->echoData)
						socket << data;
				}
			} catch (SocketException& e) {
				ostringstream message;
				message << "Exception was caught: " << e.what() << endl;

				LOG(message.str());
			} catch (EOFException& e) {
				LOG("End of stream.");
			}

			parameters->numDisconnects++;
			parameters->numConnected--;
		}
	} catch (SocketException& e) {
		ostringstream message;
		message << "Exception was caught: " << e.what() << endl;

		LOG(message.str());
	}

	parameters->numConnected = 0;

	LOG_EXIT
	return 0;
}

static string buildDataMismatchMessage(size_t index, size_t frame, size_t packetIndex, size_t packetLength, const char* packet){
	ostringstream result;

	size_t showBytesLeft = 4;
	size_t showBytesRight = 4;
	size_t leftIndex = index - showBytesLeft;
	size_t rightIndex = index + showBytesRight;

	/* adapt bytes to show from the left side */
	if(index < showBytesLeft){
	    leftIndex = 0;
	    showBytesLeft = index;
	}

	/* adapt bytes to show from the right side */
	if(packetLength - index < showBytesRight){
	        rightIndex = index + 1;
	        showBytesRight = (packetLength - 1) - index;
	 }

	result << "Data mismatch at " << (int) index << ": frame=" << (int)frame
	        << ", packetIndex=" << (int)packetIndex
	        << ", packetLength=" << (int)packetLength
	        << ", packet=" << toHex(&packet[leftIndex], showBytesLeft)
	        << "|" << toHex(&packet[index], 1)
	        << "|" << toHex(&packet[rightIndex], showBytesRight);

	return result.str();
}

/**
 * Creates a new \link TcpConnectionTest \linkend.
 */
TcpConnectionTest::TcpConnectionTest(){
    /* empty */
}

/**
 * Initializes the unit test.
 */
void TcpConnectionTest::setUp(){
    /* empty */
}

/**
 * Clean up unit test.
 */
void TcpConnectionTest::tearDown(){
    /* empty */
}

/**
 * Test for TcpConnection::TcpConnection. It is verified that:
 * <ul>
 *     <li>it complains about invalid hosts</li>
 *     <li>a connection throws SystemException when the server endpoint is not listening for incoming connections.</li>
 *     <li>it connects to 127.0.0.1:8911 when an empty name is provided</li>
 *     <li>it connects to 127.0.0.1:8911 when name is ":8911"</li>
 *     <li>it connects to 127.0.0.1:8911 when name is "127.0.0.1:8911"</li>
 *     <li>that TcpConnection disconnects when TcpConnection::disconnected is called</li>
 * </ul>
 */
void TcpConnectionTest::test_Constructor(){
    LOG_ENTER

#ifdef USE_HIPPOMOCKS
    MockRepository mockups;
    Logger* logger = (Logger*)mockups.ClassMock<LoggerMockup>();
    IReceiveCallback* callback = mockups.InterfaceMock<IReceiveCallback>();
#else    
    Logger* logger = new Logger("TcpConnectionTest");
    testReceiver* callback = new testReceiver();
#endif

	/* invalid host address */
	CPPUNIT_ASSERT_THROW(TcpConnection(callback, *logger, "xxx:10000"), tsd::common::errors::SystemException);
	CPPUNIT_ASSERT_THROW(TcpConnection(callback, *logger, ":10000"), tsd::common::errors::SystemException);

	/* connection refused */
	CPPUNIT_ASSERT_THROW(TcpConnection(callback, *logger, ""), tsd::common::errors::SystemException);
	CPPUNIT_ASSERT_THROW(TcpConnection(callback, *logger, "127.0.0.1:10000"), tsd::common::errors::SystemException);

    /* initialize server */
	ServerSocket ss(8911);
	ServerParameters parameters = { &ss, 0, 0 , 0, false, NULL };
	pthread_t serverThread;
	pthread_create( &serverThread, NULL, runServer, (void*)&parameters);

	sleep(1);

    /* open regular connection */
    /* should connect to 127.0.0.1:8911 */
    TcpConnection dut(callback, *logger, "");

    CPPUNIT_ASSERT_EQUAL((uint32_t)1, parameters.numConnected);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1, parameters.numConnects);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0, parameters.numDisconnects);

#ifdef USE_HIPPOMOCKS
    mockups.ExpectCall(callback, IReceiveCallback::disconnected);
#endif

    dut.disconnected();

    sleep(1);

    CPPUNIT_ASSERT_EQUAL((uint32_t) 0, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numDisconnects);

    /* try connecting with socket ID */
    ClientSocket s("127.0.0.1", 8911);
    TcpConnection dut2(callback, *logger, s.getSocketId());

    CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 2, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numDisconnects);

#ifdef USE_HIPPOMOCKS
    mockups.ExpectCall(callback, IReceiveCallback::disconnected);
#endif

    dut2.disconnected();

    sleep(1);

    CPPUNIT_ASSERT_EQUAL((uint32_t) 0, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 2, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 2, parameters.numDisconnects);

    /* try connecting 127.0.0.1:8911 */
	TcpConnection dut3(callback, *logger, "127.0.0.1:8911");

    CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 3, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 2, parameters.numDisconnects);

#ifdef USE_HIPPOMOCKS
	mockups.ExpectCall(callback, IReceiveCallback::disconnected);
#endif

	dut3.disconnected();

	sleep(1);

    CPPUNIT_ASSERT_EQUAL((uint32_t) 0, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 3, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 3, parameters.numDisconnects);

#ifndef USE_HIPPOMOCKS
    delete logger;
    delete callback;
#endif

	/* interrupt server */
    pthread_cancel(serverThread);
    pthread_join(serverThread,NULL);

    LOG_EXIT
}

/**
 * Test for TcpConnection::send. It is verified that:
 * <ul>
 *     <li>single frames can be send</li>
 *     <li>received data equals send one</li>
 *     <li>a native array is properly send</li>
 *     <li>a Buffer is properly send</li>
 *     <li>frame length is indicated properly</li>
 *     <li>different packet sizes can be send</li>
 * </ul>
 */
void TcpConnectionTest::test_send_SingleFrames(){
    LOG_ENTER

    const size_t bufferSize = 1024;
    const size_t headerSize = sizeof(uint32_t);
    uint8_t data[bufferSize] = { 0 };

    /* generate test data */
	for (uint32_t i = 0; i < bufferSize; i++) {
		data[i] = (uint8_t) (1 << (i % 8));
	}

#ifdef USE_HIPPOMOCKS
    MockRepository mockups;
    Logger* logger = (Logger*)mockups.ClassMock<LoggerMockup>();
    IReceiveCallback* callback = mockups.InterfaceMock<IReceiveCallback>();
#else
    Logger* logger = new Logger("TcpConnectionTest");
    testReceiver* callback = new testReceiver();
#endif

    /* initialize server */
    vector<string> receivedData;
	ServerSocket ss(10000);
	ServerParameters parameters = { &ss, 0, 0, 0, false, &receivedData };
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, runServer, (void*) &parameters);

	sleep(1);

	TcpConnection dut(callback, *logger, "127.0.0.1:10000");

	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 0, parameters.numDisconnects);

	/* send data using native buffer */

	/* send different packet sizes */
	size_t packetIndex = 0;
	for(size_t i = 1; i < bufferSize; i *= 2, packetIndex++){
		dut.send(&data[0], i);
		sleep(1); /* force single packets */
	}

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of packets received", packetIndex, parameters.receivedData->size());

	/* verify payload */
	packetIndex = 0;
	for(size_t i = 1; i < bufferSize; i *= 2, packetIndex++){
		const char* packet = receivedData[packetIndex].data();
		size_t packetLength = *((uint32_t*)&packet[0]);

		CPPUNIT_ASSERT_EQUAL_MESSAGE("Packet length", i, packetLength);

		for (size_t p = 0; p < packetLength; p++) {
			CPPUNIT_ASSERT_EQUAL_MESSAGE(
					buildDataMismatchMessage(p, packetIndex, p, packetLength, packet),
					(unsigned int)data[p], ((unsigned int) packet[p + headerSize] & 0xFF));
		}
	}

	receivedData.clear();

	/* verify sending Buffer */
	/* send different packet sizes */
	packetIndex = 0;
	for (size_t i = 1; i < bufferSize; i *= 2, packetIndex++) {
		Buffer* buffer = allocBuffer(i);
		buffer->fill(data, i);

		dut.send(buffer);

		buffer->deref();
		sleep(1); /* force single packets */
	}

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of packets received", packetIndex,
			parameters.receivedData->size());

	/* verify payload */
	packetIndex = 0;
	for (size_t i = 1; i < bufferSize; i *= 2, packetIndex++) {
		const char* packet = receivedData[packetIndex].data();
		size_t packetLength = *((uint32_t*)&packet[0]);

		CPPUNIT_ASSERT_EQUAL_MESSAGE("Packet length", i, packetLength);

		for (size_t p = 0; p < packetLength; p++) {
			CPPUNIT_ASSERT_EQUAL_MESSAGE(
					buildDataMismatchMessage(p, packetIndex, p, packetLength, packet), (unsigned int)data[p],
							((unsigned int) packet[p + headerSize] & 0xFF));
		}
	}

#ifdef USE_HIPPOMOCKS
	mockups.ExpectCall(callback, IReceiveCallback::disconnected);
#endif

	dut.disconnected();

	sleep(1);

#ifndef USE_HIPPOMOCKS
   delete logger;
   delete callback;
#endif

    /* interrupt server */
	pthread_cancel (serverThread);
	pthread_join(serverThread, NULL);

    LOG_EXIT
}

/**
 * Test for TcpConnection::send. It is verified that:
 * <ul>
 *     <li>sub-sequent calls to send forces sending one big frame</li>
 *     <li>received data equals send one</li>
 *     <li>a native array is properly send</li>
 *     <li>a Buffer is properly send</li>
 *     <li>frame length is indicated properly</li>
 *     <li>different packet sizes can be send</li>
 * </ul>
 */
void TcpConnectionTest::test_send_Burst(){
    LOG_ENTER


    const size_t bufferSize = 1024;
    const size_t headerSize = sizeof(uint32_t);
    uint8_t data[bufferSize] = { 0 };

    /* generate test data */
	for (uint32_t i = 0; i < bufferSize; i++) {
		data[i] = (uint8_t) (1 << (i % 8));
	}

#ifdef USE_HIPPOMOCKS
    MockRepository mockups;
    Logger* logger = (Logger*)mockups.ClassMock<LoggerMockup>();
    IReceiveCallback* callback = mockups.InterfaceMock<IReceiveCallback>();
#else
    Logger* logger = new Logger("TcpConnectionTest");
    testReceiver* callback = new testReceiver();
#endif

    /* initialize server */
    vector<string> receivedData;
	ServerSocket ss(10000);
	ServerParameters parameters = { &ss, 0, 0, 0, false, &receivedData };
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, runServer, (void*) &parameters);

	sleep(1);

	TcpConnection dut(callback, *logger, "127.0.0.1:10000");

	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnected);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 1, parameters.numConnects);
	CPPUNIT_ASSERT_EQUAL((uint32_t) 0, parameters.numDisconnects);


	LOG("Send data...");

	/* send data using native buffer */
	/* send different packet sizes but force a burst frame */
	for(size_t i = 1; i < bufferSize; i *= 2){
		dut.send(&data[0], i);
	}

	sleep(2); /* time to receive */

	LOG("Verify received data...");

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of frames received", (size_t)1, parameters.receivedData->size());

	/* verify payload */
    const char* frame = receivedData[0].data();

    /* iterate over packets of a frame */
    for (size_t packetIndex = 0, expectedPacketLength = 1;
            packetIndex < receivedData[0].size();
            /* will be done within loop */) {


        size_t packetLength = *((uint32_t*) &frame[packetIndex]);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Packet length", expectedPacketLength, packetLength);

        /* data starts after the packet length */
        packetIndex += headerSize;

        for (size_t d = 0; d < packetLength; d++) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                    buildDataMismatchMessage(d, 1, packetIndex,
                            packetLength, &frame[packetIndex]),
                    (unsigned int )data[d],
                    ((unsigned int ) frame[packetIndex + d] & 0xFF));
        }

        packetIndex += packetLength;
        expectedPacketLength += packetLength;
    }


	receivedData.clear();

	LOG("Send data...");

	/* verify sending Buffer */
	/* send different packet sizes but force a burst frame */
	for (size_t i = 1; i < bufferSize; i *= 2) {
		Buffer* buffer = allocBuffer(i);
		buffer->fill(data, i);

		dut.send(buffer);

		buffer->deref();
	}

	sleep(2); /* time to receive */

	LOG("Verify received data...");

	CPPUNIT_ASSERT_EQUAL_MESSAGE("Number of packets received", (size_t)1,
			parameters.receivedData->size());

	/* verify payload */
	frame = receivedData[0].data();

    /* iterate over packets of a frame */
    for (size_t packetIndex = 0, expectedPacketLength = 1;
            packetIndex < receivedData[0].size();
            /* will be done within loop */) {

        size_t packetLength = *((uint32_t*) &frame[packetIndex]);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Packet length", expectedPacketLength,
                packetLength);

        /* data starts after the packet length */
        packetIndex += headerSize;

        for (size_t d = 0; d < packetLength; d++) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                    buildDataMismatchMessage(d, 1, packetIndex, packetLength,
                            &frame[packetIndex]), (unsigned int )data[d],
                    ((unsigned int ) frame[packetIndex + d] & 0xFF));
        }

        packetIndex += packetLength;
        expectedPacketLength += packetLength;
    }

#ifdef USE_HIPPOMOCKS
	mockups.ExpectCall(callback, IReceiveCallback::disconnected);
#endif	

	dut.disconnected();

	sleep(1);

#ifndef USE_HIPPOMOCKS
   delete logger;
   delete callback;
#endif

    /* interrupt server */
	pthread_cancel (serverThread);
	pthread_join(serverThread, NULL);

    LOG_EXIT
}


CPPUNIT_TEST_SUITE_REGISTRATION(TcpConnectionTest);

} // - namespace tsd
} // - namespace communication
