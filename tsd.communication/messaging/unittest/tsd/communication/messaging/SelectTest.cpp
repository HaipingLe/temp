//////////////////////////////////////////////////////////////////////
/// @file SelectTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test Select
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include <arpa/inet.h>
#include <chrono>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <gmock/gmock.h>
#include <stdexcept>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/Select.hpp>
#include <unistd.h>

// struct sockaddr_in must be defined before including this header. Otherwise it won't compile.
#include "SelectTest.hpp"

namespace tsd {
namespace communication {
namespace messaging {

namespace {

int countFiles(const char* dir, bool ignoreDots = true, bool ignoreReadDir = true)
{
   DIR*           dp = opendir(dir);
   int            i{0};
   struct dirent* ep;
   int            dfd = dirfd(dp);
   std::string    sFd = std::to_string(dfd);

   if (dp)
   {
      while ((ep = readdir(dp)))
      {
         std::string dName{ep->d_name};

         if (ignoreReadDir && sFd == dName)
         {
            continue;
         }
         if (ignoreDots && (dName == "." || dName == ".."))
         {
            continue;
         }
         i++;
      }

      closedir(dp);
   }
   else
   {
      throw std::runtime_error("couldn't open the directory");
   }

   return i;
}

int maxFds()
{
   std::ifstream f("/proc/self/limits");
   for (std::string line; getline(f, line);)
   {
      if (line.find("Max open files") == 0)
      {
         std::stringstream ssLine(line);
         std::string       token;
         for (int i = 0; i < 4; i++)
         {
            ssLine >> token;
         }
         return std::stoi(token);
      }
   }

   return 1024;
}

class SelectTestDispatchThread : public tsd::common::system::Thread
{
   Select& m_TestObj;

public:
   SelectTestDispatchThread(Select& testObj) : Thread("SelectTestDispatchThread"), m_TestObj(testObj)
   {
   }

   void run() override
   {
      m_TestObj.dispatch();
   }
};
}

void SelectTest::setUp()
{
   m_UdpSocket              = -1;
   m_TestObj                = std::make_shared<Select>();
   m_SelectEventHandlerMock = std::make_shared<ISelectEventHandlerMock>();

   CPPUNIT_ASSERT_MESSAGE("create socket failed", (m_UdpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1);

   m_SiMeLen = sizeof(m_SiMe);
   memset((char*)&m_SiMe, 0, m_SiMeLen);
   m_SiMe.sin_family = AF_INET;
   bool boundOk      = false;
   for (uint16_t port = 10000; port > 0; port++)
   {
      m_SiMe.sin_port        = htons(port);
      m_SiMe.sin_addr.s_addr = htonl(INADDR_ANY);

      if (bind(m_UdpSocket, (struct sockaddr*)&m_SiMe, sizeof(m_SiMe)) != -1)
      {
         boundOk = true;
         break;
      }
   }

   if (!boundOk)
   {
      close(m_UdpSocket);
      m_UdpSocket = -1;
      CPPUNIT_ASSERT_MESSAGE("bind failed", false);
   }
}

void SelectTest::tearDown()
{
   if (m_UdpSocket != -1)
   {
      close(m_UdpSocket);
      m_UdpSocket = -1;
   }
   CPPUNIT_ASSERT_MESSAGE("mock verify failed", testing::Mock::VerifyAndClear(m_SelectEventHandlerMock.get()));
   tsd::common::logging::LoggingManager::cleanup();
}

void SelectTest::test_Constructor_WithoutParameters_ObjectCreated()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", std::make_shared<Select>());
}

class SmartSelectSource
{
   SelectSource* m_ptr;

public:
   SmartSelectSource(SelectSource* ptr) : m_ptr(ptr)
   {
   }
   ~SmartSelectSource()
   {
      if (m_ptr)
      {
         m_ptr->dispose();
      }
   }
   operator bool() const
   {
      return m_ptr != nullptr;
   }
};

void SelectTest::test_Add_AgainSameListener_NothingHappens()
{
   CPPUNIT_ASSERT_MESSAGE("wrong value", m_TestObj->init());
   SmartSelectSource source(m_TestObj->add(m_UdpSocket, m_SelectEventHandlerMock.get()));
   CPPUNIT_ASSERT_MESSAGE("wrong value", source);
   CPPUNIT_ASSERT_MESSAGE("wrong value", !SmartSelectSource(m_TestObj->add(m_UdpSocket, m_SelectEventHandlerMock.get())));
}

void SelectTest::test_Init_AllFdsCreatedAndRegistered_TrueReturned()
{
   CPPUNIT_ASSERT_MESSAGE("wrong value", m_TestObj->init());
}

void SelectTest::test_Init_NotEnoughDescriptorsForMwakeFd_FalseReturned()
{
   int limit = maxFds() - countFiles("/proc/self/fd") - 1;
   int fds[limit];
   for (int i = 0; i < limit; i++)
   {
      fds[i] = epoll_create1(EPOLL_CLOEXEC);
   }

   CPPUNIT_ASSERT_MESSAGE("wrong value", !m_TestObj->init());

   for (int i = 0; i < limit; i++)
   {
      close(fds[i]);
   }
}

void SelectTest::test_Init_NotEnoughDescriptors_FalseReturned()
{
   int limit = maxFds() - countFiles("/proc/self/fd");
   int fds[limit];
   for (int i = 0; i < limit; i++)
   {
      fds[i] = epoll_create1(EPOLL_CLOEXEC);
   }

   CPPUNIT_ASSERT_MESSAGE("wrong value", !m_TestObj->init());

   for (int i = 0; i < limit; i++)
   {
      close(fds[i]);
   }
}

void SelectTest::test_Dispatch_EpollWaitFailed_NothingHappened()
{
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("dispatch failed", m_TestObj->dispatch());
}

void SelectTest::test_Dispatch_WaitForWriteable_SelectWriteableCalled()
{
   CPPUNIT_ASSERT_MESSAGE("wrong value", m_TestObj->init());
   SelectTestDispatchThread dispatchThread(*m_TestObj);
   EXPECT_CALL(*m_SelectEventHandlerMock, selectWritable()).WillOnce(testing::Return(false));
   SmartSelectSource source(m_TestObj->add(m_UdpSocket, m_SelectEventHandlerMock.get(), false));
   CPPUNIT_ASSERT_MESSAGE("wrong value", source);
   dispatchThread.start();

   m_TestObj->interrupt();
   dispatchThread.join();
}

void SelectTest::test_Dispatch_WaitForReadable_SelectReadableCalled()
{
   CPPUNIT_ASSERT_MESSAGE("wrong value", m_TestObj->init());
   SelectTestDispatchThread dispatchThread(*m_TestObj);

   EXPECT_CALL(*m_SelectEventHandlerMock, selectReadable()).WillOnce(testing::Return(false));
   CPPUNIT_ASSERT_MESSAGE("sendto failed", sendto(m_UdpSocket, m_Buffer, sizeof(m_Buffer), 0, (struct sockaddr*)&m_SiMe, m_SiMeLen) != -1);
   SmartSelectSource source(m_TestObj->add(m_UdpSocket, m_SelectEventHandlerMock.get(), true, false));
   CPPUNIT_ASSERT_MESSAGE("wrong value", source);

   dispatchThread.start();
   m_TestObj->interrupt();
   dispatchThread.join();
}

void SelectTest::test_Dispatch_WaitForHangUp_SelectErrorCalled()
{
   CPPUNIT_ASSERT_MESSAGE("wrong value", m_TestObj->init());
   SelectTestDispatchThread dispatchThread(*m_TestObj);

   CPPUNIT_ASSERT_MESSAGE("sendto failed", sendto(m_UdpSocket, m_Buffer, 0, 0, (struct sockaddr*)&m_SiMe, m_SiMeLen) != -1);
   EXPECT_CALL(*m_SelectEventHandlerMock, selectError()).WillOnce(testing::Return(false));
   SmartSelectSource source(m_TestObj->add(m_UdpSocket, m_SelectEventHandlerMock.get(), false, false));
   CPPUNIT_ASSERT_MESSAGE("wrong value", source);

   shutdown(m_UdpSocket, SHUT_RDWR);
   dispatchThread.start();
   m_TestObj->interrupt();
   dispatchThread.join();
}

void SelectTest::test_Dispatch_WaitForWriteableAndReadableReadableReceived_ExpectationsModified()
{
   CPPUNIT_ASSERT_MESSAGE("wrong value", m_TestObj->init());
   SelectTestDispatchThread dispatchThread(*m_TestObj);

   EXPECT_CALL(*m_SelectEventHandlerMock, selectReadable()).WillOnce(testing::Return(false));
   EXPECT_CALL(*m_SelectEventHandlerMock, selectWritable()).WillOnce(testing::Return(false));

   SmartSelectSource source(m_TestObj->add(m_UdpSocket, m_SelectEventHandlerMock.get()));
   CPPUNIT_ASSERT_MESSAGE("wrong value", source);
   CPPUNIT_ASSERT_MESSAGE("sendto failed", sendto(m_UdpSocket, m_Buffer, 0, 0, (struct sockaddr*)&m_SiMe, m_SiMeLen) != -1);
   dispatchThread.start();

   m_TestObj->interrupt();
   dispatchThread.join();
}

CPPUNIT_TEST_SUITE_REGISTRATION(SelectTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
