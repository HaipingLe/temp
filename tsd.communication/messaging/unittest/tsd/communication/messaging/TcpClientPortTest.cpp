//////////////////////////////////////////////////////////////////////
/// @file TcpClientPortTest.cpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Unit Tests to test TcpClientPort
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include "TcpClientPortTest.hpp"
#include <dirent.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/Select.hpp>
#include <tsd/communication/messaging/TcpClientPort.hpp>
#include <tsd/communication/messaging/TcpEndpointMock.hpp>
#include <tsd/communication/messaging/TcpServerPort.hpp>
#include <unistd.h>

namespace tsd {
namespace communication {
namespace messaging {

namespace {

class FileDescriptorSeizer
{
   std::vector<int> m_fds;

public:
   FileDescriptorSeizer(uint8_t toSpare)
   {
      m_fds.reserve(sysconf(_SC_OPEN_MAX));
      do
      {
         m_fds.push_back(epoll_create1(EPOLL_CLOEXEC));
      } while (m_fds.back() != -1);
      m_fds.pop_back();
      for (; (toSpare > 0) && !m_fds.empty(); --toSpare)
      {
         close(m_fds.back());
         m_fds.pop_back();
      }
   }
   ~FileDescriptorSeizer()
   {
      while (!m_fds.empty())
      {
         close(m_fds.back());
         m_fds.pop_back();
      }
   }
};
}

void TcpClientPortTest::setUp()
{
}

void TcpClientPortTest::tearDown()
{
   tsd::common::logging::LoggingManager::cleanup();
}

void TcpClientPortTest::test_Constructor_WithRouterAndSubDomain_ObjectCreated()
{
   tsd::communication::messaging::Router router("router");
   const std::string                     subDomain = "test";

   // test execution
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("Constructor should not throw", std::make_shared<TcpClientPort>(router, subDomain));
}

void TcpClientPortTest::test_Disconnect_JustRun_NothingHappens()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);

   // test execution
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", (testObj.disconnect()));
}

void TcpClientPortTest::test_InitV4_InitFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   uint32_t addr   = 0;
   uint16_t port   = 0;
   uint32_t sndBuf = 0;
   uint32_t rcvBuf = 0;

   // to make sure that init will fail
   FileDescriptorSeizer seizer(1);

   // test execution
   try
   {
      testObj.initV4(addr, port, sndBuf, rcvBuf);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "Selector init failed!";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitV4_SocketCreationFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   uint32_t addr   = 0;
   uint16_t port   = 0;
   uint32_t sndBuf = 0;
   uint32_t rcvBuf = 0;

   // to make sure that socket will fail
   FileDescriptorSeizer seizer(2);

   // test execution
   try
   {
      testObj.initV4(addr, port, sndBuf, rcvBuf);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "Could not create socket: Too many open files";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitV4_NoConnection_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   uint32_t addr   = 0;
   uint16_t port   = 0;
   uint32_t sndBuf = 0;
   uint32_t rcvBuf = 0;

   // test execution
   try
   {
      testObj.initV4(addr, port, sndBuf, rcvBuf);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "connect failed: Connection refused";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitV4_TcpEndpointInitFailed_ConnectionExceptionThrown()
{
   /* unable to fail TcpEndpoint init*/
   //   tsd::communication::messaging::Router router("router");
   //   //function under test parameter preparation:
   //   uint32_t addr = 0x7F000001;
   //   uint32_t sndBuf = 1;
   //   uint32_t rcvBuf = 1;
   //   TcpServerPort server(router);
   //   server.initV4(addr);
   //   TcpClientPort testObj(router);
   //   //it is expected that setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&size,sizeof(size))!=0
   //   //it is expected that setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size))!=0
   //   //it is expected that !TcpEndpoint::init(sock,m_select)
   //   //throw ConnectionException ("TcpEndpoint init failed")

   //   //test execution
   //   try
   //   {
   //      testObj.initV4(addr, server.getBoundPort(), sndBuf, rcvBuf);
   //   }
   //   catch (ConnectionException e)
   //   {
   //      std::string expectedText = "TcpEndpoint init failed";
   //      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
   //      return;
   //   }
   //   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitV4_UpstreamInitFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   // function under test parameter preparation:
   uint32_t addr   = 0x7F000001;
   uint32_t sndBuf = 1;
   uint32_t rcvBuf = 1;
   // it is expected that setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&size,sizeof(size))==0
   // it is expected that setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size))==0
   TcpServerPort server(router);
   server.initV4(addr);
   TcpClientPort testObj(router);

   // test execution
   try
   {
      testObj.initV4(addr, server.getBoundPort(), sndBuf, rcvBuf);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "initUpstream failed";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitV4_NoErrors_ConnectionEstablished()
{
   tsd::communication::messaging::Router router("router");
   // function under test parameter preparation:
   uint32_t      addr   = 0x7F000001;
   uint32_t      sndBuf = 0;
   uint32_t      rcvBuf = 0;
   TcpClientPort testObj(router);
   Router        r2("r2");
   TcpServerPort server(r2);
   server.initV4(addr);

   // test execution
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions",
                                   (testObj.initV4(addr, server.getBoundPort(), sndBuf, rcvBuf)));
}

void TcpClientPortTest::test_InitUnix_SelectorInitFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   const std::string    path;
   FileDescriptorSeizer seizer(1);

   // test execution
   try
   {
      testObj.initUnix(path);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "Selector init failed!";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitUnix_SocketCreationFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   const std::string path;

   FileDescriptorSeizer seizer(2);
   // test execution
   try
   {
      testObj.initUnix(path);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "Could not create socket: Too many open files";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitUnix_ConnectionFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   const std::string path;

   // test execution
   try
   {
      testObj.initUnix(path);
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "connect failed: Connection refused";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitUnix_TcpEndpointFailed_ConnectionExceptionThrown()
{
   /* unable to fail TcpEndpoint init*/
   //   tsd::communication::messaging::Router router("router");
   //   TcpClientPort testObj(router);
   //   //function under test parameter preparation:
   //   const std::string path = "@";
   //   Router r2("r2");
   //   TcpServerPort server(r2);
   //   server.initUnix(path);

   //   //test execution
   //   try
   //   {
   //      testObj.initUnix(server.getBoundPath());
   //   }
   //   catch (ConnectionException e)
   //   {
   //      std::string expectedText = "TcpEndpoint init failed";
   //      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
   //      return;
   //   }
   //   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitUnix_UpstreamInifFailed_ConnectionExceptionThrown()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   const std::string path = "@";
   TcpServerPort     server(router);
   server.initUnix(path);

   // test execution
   try
   {
      testObj.initUnix(server.getBoundPath());
   }
   catch (ConnectionException e)
   {
      std::string expectedText = "initUpstream failed";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong Exception caught", expectedText, std::string(e.what()));
      return;
   }
   CPPUNIT_ASSERT_MESSAGE("It is expected that this call will be terminated by an exception", false);
}

void TcpClientPortTest::test_InitUnix_NoErrors_ConnectionEstablished()
{
   tsd::communication::messaging::Router router("router");
   TcpClientPort                         testObj(router);
   // function under test parameter preparation:
   const std::string path = "@";
   Router            r2("r2");
   TcpServerPort     server(r2);
   server.initUnix(path);

   // test execution
   CPPUNIT_ASSERT_NO_THROW_MESSAGE("It is expected that this method does not throw exceptions", (testObj.initUnix(server.getBoundPath())));
}

CPPUNIT_TEST_SUITE_REGISTRATION(TcpClientPortTest);
} // namespace messaging
} // namespace communication
} // namespace tsd
