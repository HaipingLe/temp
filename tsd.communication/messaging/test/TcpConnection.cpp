
#include <map>

#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>

#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/Connection.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/Queue.hpp>

// internal API
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>
#include <tsd/communication/messaging/TcpClientPort.hpp>
#include <tsd/communication/messaging/TcpServerPort.hpp>

#include "MessageDefs.hpp"
#include "TransportSuite.hpp"

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

namespace {

   class TcpServerWrapper
   {
      Router &m_router;
      uint32_t m_users;
      std::auto_ptr<TcpServerPort> m_port;
      tsd::common::system::Mutex m_lock;
      uint16_t m_tcpPort;

   public:
      TcpServerWrapper(Router &router);
      ~TcpServerWrapper();

      IConnection* createConnection(Router &south, const std::string &subDomain);
      void delConnection();
   };

   class TcpClientWrapper
      : public IConnection
   {
      TcpServerWrapper &m_srvWrap;
      uint16_t m_serverPort;
      TcpClientPort *m_client;

   public:
      TcpClientWrapper(TcpServerWrapper &srvWrap, uint16_t port);
      ~TcpClientWrapper();

      void init(Router &south, const std::string &subDomain);
      void disconnect(); // IConnection
   };

}

/*****************************************************************************/

TcpServerWrapper::TcpServerWrapper(Router &router)
   : m_router(router)
   , m_users(0)
   , m_tcpPort(0)
{ }

TcpServerWrapper::~TcpServerWrapper()
{ }

IConnection* TcpServerWrapper::createConnection(Router &south, const std::string &subDomain)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_users == 0) {
      std::auto_ptr<TcpServerPort> port(new TcpServerPort(m_router));
      port->initV4(INADDR_LOOPBACK, 0, 4000, 4000);
      m_tcpPort = port->getBoundPort();
      CPPUNIT_ASSERT(m_tcpPort != 0);
      m_port = port;
   }
   m_users++;

   // create wrapper and initilialize asynchronously
   std::auto_ptr<TcpClientWrapper> conn(new TcpClientWrapper(*this, m_tcpPort));
   g.unlock();
   conn->init(south, subDomain);
   return conn.release();
}

void TcpServerWrapper::delConnection()
{
   tsd::common::system::MutexGuard g(m_lock);

   CPPUNIT_ASSERT(m_users > 0);
   m_users--;
   if (m_users == 0) {
      m_port.reset();
   }
}

/*****************************************************************************/

TcpClientWrapper::TcpClientWrapper(TcpServerWrapper &srvWrap, uint16_t port)
   : m_srvWrap(srvWrap)
   , m_serverPort(port)
   , m_client(0)
{
}

TcpClientWrapper::~TcpClientWrapper()
{
   TcpClientWrapper::disconnect();
   m_srvWrap.delConnection();
}

void TcpClientWrapper::init(Router &south, const std::string &subDomain)
{
   std::auto_ptr<TcpClientPort> clientPort(new TcpClientPort(south, subDomain));
   clientPort->initV4(INADDR_LOOPBACK, m_serverPort, 4000, 4000);

   m_client = clientPort.release();
}

void TcpClientWrapper::disconnect()
{
   if (m_client != 0) {
      m_client->disconnect();
      delete m_client;
      m_client = 0;
   }
}

/*****************************************************************************/

class TcpConnectionTestSuite
   : public TransportSuite
{
   typedef std::map<Router*, TcpServerWrapper*> ServerMap;
   ServerMap m_servers;
   tsd::common::system::Mutex m_lock;

   CPPUNIT_TEST_SUB_SUITE(TcpConnectionTestSuite, TransportSuite);

   // special tests from TransportSuite
   CPPUNIT_TEST(test_bulk_multi);
   CPPUNIT_TEST(test_bulk_connect_disconnect);

   // local tests
   CPPUNIT_TEST(test_connect_std_client);
   CPPUNIT_TEST(test_connect_std_server);
   CPPUNIT_TEST(test_connect_throws);
   CPPUNIT_TEST_SUITE_END();

protected:
   /**
    * Create connection between north router (server) and south router
    * (client).
    *
    * To make the tests more realistic we make sure that there is only one
    * TcpServerPort instance per Router.
    *
    * Reimplemented from TransportSuite.
    */
   IConnection* createConnection(Router &north, Router &south, const std::string &subDomain)
   {
      TcpServerWrapper *srvWrap;

      {
         // make thread safe
         tsd::common::system::MutexGuard g(m_lock);

         ServerMap::iterator it(m_servers.find(&north));
         if (it == m_servers.end()) {
            m_servers[&north] = srvWrap = new TcpServerWrapper(north);
         } else {
            srvWrap = it->second;
         }
      }

      return srvWrap->createConnection(south, subDomain);
   }

public:
   // TestFixture
   void tearDown()
   {
      for (ServerMap::iterator it(m_servers.begin()); it != m_servers.end(); ++it) {
         delete it->second;
      }
   }

   /**
    * Test TCP connection through standard interface.
    *
    * Open a listening socket on an ephemeral port and connect the local router
    * to that.
    */
   void test_connect_std_client()
   {
      Router rootRouter("root");

      // open server on ephemeral port / localhost
      TcpServerPort rootServer(rootRouter);
      rootServer.initV4(INADDR_LOOPBACK, 0);
      uint16_t port = rootServer.getBoundPort();
      CPPUNIT_ASSERT(port != 0);

      // construct URL and connect
      std::stringstream url;
      url << "tcp://127.0.0.1:" << port;
      std::auto_ptr<IConnection> conn(connectUpstream(url.str()));

      CPPUNIT_ASSERT(conn.get() != NULL);
      conn->disconnect();
   }

   /**
    * Test TCP connection to server established by standard interface.
    *
    * Open a standard listening socket and connect with separate router.
    */
   void test_connect_std_server()
   {
      std::auto_ptr<IConnection> srv(listenDownstream("tcp://127.0.0.1:54758"));
      CPPUNIT_ASSERT(srv.get() != NULL);

      Router leafRouter("leaf");
      TcpClientPort leafClient(leafRouter);
      leafClient.initV4(INADDR_LOOPBACK, 54758);

      srv->disconnect();
   }

   /**
    * Test that connection to invalid server throws exception.
    */
   void test_connect_throws()
   {
      std::auto_ptr<IConnection> conn;

      CPPUNIT_ASSERT_THROW(conn.reset(connectUpstream("tcp://127.0.0.1:9999")),
         ConnectionException);
   }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TcpConnectionTestSuite);
