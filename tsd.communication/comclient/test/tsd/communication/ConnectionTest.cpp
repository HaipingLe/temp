////////////////////////////////////////////////////////////////////////////////
///  @file ConnectionTest.cpp
///  @brief Test implementation for Connection
///  @author user@technisat.de
///  @todo repair this test whith USE_HIPPOMOCKS=1
///  Copyright (c) TechniSat Digital GmbH,
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <pthread.h>

#ifdef LOG_DISABLED
	#define LOG(message)
#else
	#define LOG_CLASS "Connection"
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

// #define USE_HIPPOMOCKS

#ifdef USE_HIPPOMOCKS
#include <hippomocks.h>
#endif
#include <iostream>

#include "net/ServerSocket.hpp"
#include "net/SocketException.hpp"
#include "net/EOFException.hpp"

//the unit test header
#include "ConnectionTest.hpp"

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

	LOG_EXIT
	return 0;
}

/**
 * Creates a new \link ConnectionTest \linkend.
 */
ConnectionTest::ConnectionTest(){
    /* empty */
}

/**
 * Initializes the unit test.
 */
void ConnectionTest::setUp(){
    /* empty */
}

/**
 * Clean up unit test.
 */
void ConnectionTest::tearDown(){
    /* empty */
}

/**
 * Test for Connection::openConnection. It is verified that:
 * <ul>
 *     <li>the method complains about invalid URLs</li>
 *     <li>"qnx://" URLs are only accepted on QNX plattforms</li>
 *     <li>a connection throws SystemException when the server endpoint is not listening for incoming connections.</li>
 *     <li></li>
 *     <li></li>
 *     <li></li>
 *     <li></li>
 * </ul>
 */
void ConnectionTest::test_openConnection(){
    LOG_ENTER

#ifdef USE_HIPPOMOCKS
    MockRepository mockups;
    Logger* logger = (Logger*)mockups.ClassMock<LoggerMockup>();
    IReceiveCallback* callback = mockups.InterfaceMock<IReceiveCallback>();
#else
    Logger* logger = new Logger("ConnectionTest");
    testReceiver* callback = new testReceiver();
#endif
    Connection* dut = NULL;

    /* verify that invalid URLs are detected */
    LOG("Verify invalid URL: '://'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "://")), tsd::common::errors::ConnectException);
    delete dut;

    LOG("Verify invalid URL: 'a://'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "a://")), tsd::common::errors::ConnectException);
    delete dut;

    LOG("Verify invalid URL: 'tcp ://'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "tcp ://")), tsd::common::errors::ConnectException);
    delete dut;
#ifndef TARGET_OS_POSIX_QNX
    /* qnx:// should only be available on QNX plattforms */
    LOG("Verify invalid URL: 'qnx://'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "qnx://")), tsd::common::errors::ConnectException);
    delete dut;
#endif

    /* invalid host address */
    LOG("Verify invalid host: 'NULL'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, NULL)), tsd::common::errors::SystemException);
    delete dut;

    LOG("Verify invalid host: 'tcp://xxx:10000'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "tcp://xxx:10000")), tsd::common::errors::SystemException);
    delete dut;

    /* connection refused */
    LOG("Verify connection refused: '127.0.0.1:10000'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "127.0.0.1:10000")), tsd::common::errors::SystemException);
    delete dut;

    LOG("Verify connection refused: 'tcp://127.0.0.1:10000'");
    CPPUNIT_ASSERT_THROW((dut = Connection::openConnection(callback, *logger, "tcp://127.0.0.1:10000")), tsd::common::errors::SystemException);
    delete dut;

    ServerSocket serverSocket(10000);
    ServerParameters parameters = { &serverSocket, 0, 0 , 0 };
    pthread_t serverThread;
    pthread_create( &serverThread, NULL, runServer, (void*)&parameters);

    sleep(1);

    /* open regular connection */
    LOG("Verify open: 'tcp://127.0.0.1:10000'");
    dut = Connection::openConnection(callback, *logger, "tcp://127.0.0.1:10000");

    TcpConnection* connection = dynamic_cast<TcpConnection*>(dut);
    CPPUNIT_ASSERT(connection != NULL);

    CPPUNIT_ASSERT_EQUAL((uint32_t)1, parameters.numConnected);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1, parameters.numConnects);
    CPPUNIT_ASSERT_EQUAL((uint32_t)0, parameters.numDisconnects);

    LOG("Disconnect...");

#ifdef USE_HIPPOMOCKS
    mockups.ExpectCall(callback, IReceiveCallback::disconnected);
    connection->disconnected();
#endif

    delete dut;
#ifndef USE_HIPPOMOCKS
    delete logger;
    delete callback;
#endif

    sleep(1);

    CPPUNIT_ASSERT_EQUAL((uint32_t)0, parameters.numConnected);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1, parameters.numConnects);
    CPPUNIT_ASSERT_EQUAL((uint32_t)1, parameters.numDisconnects);

    /* interrupt server */
    pthread_cancel(serverThread);
    pthread_join(serverThread,NULL);

    LOG_EXIT
}

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionTest);

} // - namespace tsd
} // - namespace communication
