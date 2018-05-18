//////////////////////////////////////////////////////////////////////
/// @file TcpEndpointTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test TcpEndpoint
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#define _GLIBCXX_TR1_TUPLE 0
#define _GLIBCXX_TR1_FUNCTIONAL 0

#include "TcpEndpointTest.hpp"

#include <arpa/inet.h>
#include <chrono>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/Packet.hpp>
#include <tsd/communication/messaging/TcpEndpoint.hpp>

namespace tsd {
namespace communication {
namespace messaging {
namespace {
const uint32_t                             DEFAULT_EVENT_ID{1};
const Packet::Type                         DEFAULT_TYPE{Packet::OOB_BASE};
const tsd::communication::event::IfcAddr_t DEFAULT_SENDER{1};
const tsd::communication::event::IfcAddr_t DEFAULT_RECEIVER{2};
const char*                                DEFAULT_BUFFER{"testbuffer"};

class SelectDispatchThread : public tsd::common::system::Thread
{
   Select& m_Select;

public:
   SelectDispatchThread(Select& select) : Thread("SelectDispatchThread"), m_Select(select)
   {
   }

   void run() override
   {
      m_Select.dispatch();
   }
};

class ServerThread : public tsd::common::system::Thread
{
public:
   uint16_t                       m_proto;
   int                            m_SockFd, m_NewSockFd;
   socklen_t                      m_Clilen;
   char                           m_Buffer[256];
   struct sockaddr_in             m_ServAddr, m_CliAddr;
   tsd::common::system::Semaphore m_sem;

   ServerThread() : Thread("ServerThread"), m_sem(0)
   {
      m_SockFd = socket(AF_INET, SOCK_STREAM, 0);
      CPPUNIT_ASSERT_MESSAGE("Socket failed", m_SockFd != -1);
      bzero((char*)&m_ServAddr, sizeof(m_ServAddr));

      m_ServAddr.sin_family      = AF_INET;
      m_ServAddr.sin_addr.s_addr = INADDR_ANY;

      m_proto = 0;
      for (uint16_t i = 10000; i < 64000; ++i)
      {
         m_ServAddr.sin_port = htons(i);
         if (bind(m_SockFd, (struct sockaddr*)&m_ServAddr, sizeof(m_ServAddr)) != -1)
         {
            m_proto = i;
            break;
         }
      }
      CPPUNIT_ASSERT_MESSAGE("No port found", uint16_t{0} != m_proto);
   }

   void run() override
   {
      listen(m_SockFd, 5);
      m_Clilen = sizeof(m_CliAddr);

      m_sem.up();
      m_NewSockFd = accept(m_SockFd, (struct sockaddr*)&m_CliAddr, &m_Clilen);
      CPPUNIT_ASSERT_MESSAGE("Accept failed", m_NewSockFd != -1);

      bzero(m_Buffer, 256);

      while (1)
      {
         ssize_t bytes = read(m_NewSockFd, m_Buffer, 255);
         if (bytes < 1) break;
         CPPUNIT_ASSERT_MESSAGE("Write failed", write(m_NewSockFd, m_Buffer, strlen(m_Buffer)) != -1);
      }

      close(m_NewSockFd);
      close(m_SockFd);
   }

   bool openClientSocket(int& sockFd, bool blocking = true)
   {
      m_sem.down();
      bool ret = false;

      sockFd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockFd != -1)
      {
         int flags = fcntl(sockFd, F_GETFL, 0);

         if (flags >= 0)
         {
            flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

            if (0 == fcntl(sockFd, F_SETFL, flags))
            {
               bzero((char*)&m_ServAddr, sizeof(m_ServAddr));

               m_ServAddr.sin_family = AF_INET;
               m_ServAddr.sin_port   = htons(m_proto);
               uint8_t LOCALHOST[]   = {0xFF, 0x00, 0x00, 0x01};
               memcpy(LOCALHOST, &m_ServAddr.sin_addr, sizeof(m_ServAddr.sin_addr));

               if (connect(sockFd, (struct sockaddr*)&m_ServAddr, sizeof(m_ServAddr)) != -1)
               {
                  std::cout << "ERRNO: " << errno << std::endl;
               }

               ret = true;
            }
         }
      }

      return ret;
   }
};
}

TcpEndpointTest::~TcpEndpointTest()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void TcpEndpointTest::setUp()
{
   std::string prefix = "TcpEndpointTest";
   m_Select           = std::make_shared<Select>();
   m_Event            = std::make_shared<tsd::communication::event::TsdEvent>(DEFAULT_EVENT_ID);

   m_Logger  = std::make_shared<tsd::common::logging::Logger>(prefix);
   m_TestObj = std::make_shared<TcpEndpointMock>(*m_Logger.get());
}

void TcpEndpointTest::tearDown()
{
   m_TestObj.reset();
}

void TcpEndpointTest::test_EpSendPacket_WithEmptySendQueueAndZeroOffset_TrueReturned()
{
   m_Packet.reset(new Packet(m_Event.get(), false));

   bool ret = m_TestObj->epSendPacket(m_Packet);

   CPPUNIT_ASSERT_MESSAGE("First packet send failed", ret);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_EpSendPacket_CallTwiceWithZeroOffset_TrueReturned()
{
   m_Packet.reset(new Packet(m_Event.get(), false));

   bool ret1 = m_TestObj->epSendPacket(m_Packet);
   bool ret2 = m_TestObj->epSendPacket(m_Packet);

   CPPUNIT_ASSERT_MESSAGE("First packet send failed", ret1);
   CPPUNIT_ASSERT_MESSAGE("Second packet send failed", ret2);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_EpSendPacket_WithNegativeOffset_TrueReturned()
{
   m_Packet.reset(new Packet(m_Event.get(), false));

   ServerThread server;
   server.start();
   server.openClientSocket(m_SockFd, true);
   bool initRet = m_TestObj->init(m_SockFd, *m_Select.get());
   bool sendRet = m_TestObj->epSendPacket(m_Packet);
   m_TestObj->cleanup();
   close(m_SockFd);
   server.join();

   CPPUNIT_ASSERT_MESSAGE("Test object was initialized", !initRet);
   CPPUNIT_ASSERT_MESSAGE("First packet send failed", sendRet);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_EpSendPacket_WithNotEmptyBufferLenght_TrueReturned()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
   m_Packet.reset(new Packet(DEFAULT_TYPE, DEFAULT_SENDER, DEFAULT_RECEIVER, DEFAULT_EVENT_ID, DEFAULT_BUFFER, strlen(DEFAULT_BUFFER)));
#pragma GCC diagnostic pop

   ServerThread server;
   server.start();
   server.openClientSocket(m_SockFd, true);
   bool initRet = m_TestObj->init(m_SockFd, *m_Select.get());
   bool sendRet = m_TestObj->epSendPacket(m_Packet);
   m_TestObj->cleanup();
   close(m_SockFd);
   server.join();

   CPPUNIT_ASSERT_MESSAGE("Test object was initialized", !initRet);
   CPPUNIT_ASSERT_MESSAGE("First packet send failed", sendRet);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_Init_RunTwice_ReturnedFalseTwice()
{
   ServerThread server;
   server.start();
   server.openClientSocket(m_SockFd, true);
   bool initRet1 = m_TestObj->init(m_SockFd, *m_Select.get());
   m_TestObj->cleanup();
   bool initRet2 = m_TestObj->init(m_SockFd, *m_Select.get());
   m_TestObj->cleanup();
   close(m_SockFd);
   server.join();

   CPPUNIT_ASSERT_MESSAGE("Test object was initialized", !initRet1);
   CPPUNIT_ASSERT_MESSAGE("Test object was initialized", !initRet2);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_Cleanup_SelectNullAndSocketInvalid_NothingHappens()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->cleanup());

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_Cleanup_SelectNotNullAndSocketValid_NothingHappens()
{
   ServerThread server;
   server.start();
   server.openClientSocket(m_SockFd, true);
   m_TestObj->init(m_SockFd, *m_Select.get());
   close(m_SockFd);
   server.join();

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", m_TestObj->cleanup());

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_SelectWritable_WithNegativeOffset_NoExceptionThrown()
{
   ServerThread server;
   server.start();
   server.openClientSocket(m_SockFd, true);

   m_Packet.reset(new Packet(m_Event.get(), false));

   bool send         = static_cast<TcpEndpointMock*>(m_TestObj.get())->epSendPacket(m_Packet);
   bool initSelector = m_Select->init();
   bool initObject   = m_TestObj->init(m_SockFd, *m_Select.get());

   SelectDispatchThread selectDispatchThread(*m_Select.get());
   selectDispatchThread.start();
   std::this_thread::sleep_for(std::chrono::milliseconds{500});

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Select writable threw exception",
                                   m_Select->add(m_SockFd, reinterpret_cast<ISelectEventHandler*>(m_TestObj.get()), EPOLLOUT));

   std::this_thread::sleep_for(std::chrono::milliseconds{500});

   m_Select->interrupt();
   selectDispatchThread.join();

   close(m_SockFd);
   server.join();

   CPPUNIT_ASSERT_MESSAGE("Send have failed", send);
   CPPUNIT_ASSERT_MESSAGE("Selector init returned false", initSelector);
   CPPUNIT_ASSERT_MESSAGE("Test object init returned false", initObject);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_SelectReadable_WithNotEmptyMessage_NoExceptionThrown()
{
   ServerThread server;
   server.start();
   bool open = server.openClientSocket(m_SockFd, false);

   m_Packet.reset(new Packet(m_Event.get(), false));

   bool send         = static_cast<TcpEndpointMock*>(m_TestObj.get())->epSendPacket(m_Packet);
   bool initSelector = m_Select->init();
   bool initObject   = m_TestObj->init(m_SockFd, *m_Select.get());

   SelectDispatchThread selectDispatchThread(*m_Select.get());
   selectDispatchThread.start();
   std::this_thread::sleep_for(std::chrono::milliseconds{100});

   std::string testMessage = "TestMessage";
   ssize_t     writeRes    = write(m_SockFd, testMessage.c_str(), strlen(testMessage.c_str()));
   std::this_thread::sleep_for(std::chrono::milliseconds{100});

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Select writable threw exception",
                                   m_Select->add(m_SockFd, reinterpret_cast<ISelectEventHandler*>(m_TestObj.get()), EPOLLIN));

   std::this_thread::sleep_for(std::chrono::milliseconds{100});

   shutdown(m_SockFd, SHUT_RDWR);
   shutdown(server.m_SockFd, SHUT_RDWR);
   close(m_SockFd);
   close(server.m_SockFd);

   std::this_thread::sleep_for(std::chrono::milliseconds{100});
   m_Select->interrupt();

   std::this_thread::sleep_for(std::chrono::milliseconds{100});
   selectDispatchThread.join();

   std::this_thread::sleep_for(std::chrono::milliseconds{100});
   server.join();

   CPPUNIT_ASSERT_MESSAGE("Port was not opened", open);
   CPPUNIT_ASSERT_MESSAGE("Send have failed", send);
   CPPUNIT_ASSERT_MESSAGE("Selector init returned false", initSelector);
   CPPUNIT_ASSERT_MESSAGE("Test object init returned false", initObject);
   CPPUNIT_ASSERT_MESSAGE("Write failed", writeRes != -1);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}

void TcpEndpointTest::test_SelectError_WithNegativeOffset_NoExceptionThrown()
{
   ServerThread server;
   server.start();
   bool open = server.openClientSocket(m_SockFd, true);

   EXPECT_CALL(*static_cast<TcpEndpointMock*>(m_TestObj.get()), epDisconnected());

   m_Packet.reset(new Packet(m_Event.get(), false));

   bool send         = static_cast<TcpEndpointMock*>(m_TestObj.get())->epSendPacket(m_Packet);
   bool initSelector = m_Select->init();
   bool initObject   = m_TestObj->init(m_SockFd, *m_Select.get());

   SelectDispatchThread selectDispatchThread(*m_Select.get());
   selectDispatchThread.start();
   std::this_thread::sleep_for(std::chrono::milliseconds{500});

   shutdown(m_SockFd, SHUT_RDWR);

   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Select writable threw exception",
                                   m_Select->add(m_SockFd, reinterpret_cast<ISelectEventHandler*>(m_TestObj.get()), EPOLLHUP));

   std::this_thread::sleep_for(std::chrono::milliseconds{500});

   m_Select->interrupt();
   selectDispatchThread.join();

   close(m_SockFd);
   server.join();

   CPPUNIT_ASSERT_MESSAGE("Port was not opened", open);
   CPPUNIT_ASSERT_MESSAGE("False returned", send);
   CPPUNIT_ASSERT_MESSAGE("Selector init returned false", initSelector);
   CPPUNIT_ASSERT_MESSAGE("False returned", initObject);

   CPPUNIT_ASSERT_MESSAGE("Mock was not verified",
                          ::testing::Mock::VerifyAndClearExpectations(static_cast<TcpEndpointMock*>(m_TestObj.get())));
}
CPPUNIT_TEST_SUITE_REGISTRATION(TcpEndpointTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
