
#include <map>

#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>

#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/common/utils/FileOperations.hpp>
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

   class UnixServerWrapper
   {
      Router &m_router;
      uint32_t m_users;
      std::auto_ptr<TcpServerPort> m_port;
      tsd::common::system::Mutex m_lock;
      std::string m_path;

   public:
      UnixServerWrapper(Router &router);
      ~UnixServerWrapper();

      IConnection* createConnection(Router &south, const std::string &subDomain);
      void delConnection();
   };

   class UnixClientWrapper
      : public IConnection
   {
      UnixServerWrapper &m_srvWrap;
      std::string m_serverPath;
      TcpClientPort *m_client;

   public:
      UnixClientWrapper(UnixServerWrapper &srvWrap, const std::string &serverPath);
      ~UnixClientWrapper();

      void init(Router &south, const std::string &subDomain);
      void disconnect(); // IConnection
   };

}

/*****************************************************************************/

UnixServerWrapper::UnixServerWrapper(Router &router)
   : m_router(router)
   , m_users(0)
{ }

UnixServerWrapper::~UnixServerWrapper()
{
   CPPUNIT_ASSERT(m_port.get() == NULL);
}

IConnection* UnixServerWrapper::createConnection(Router &south, const std::string &subDomain)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_users == 0) {
      std::auto_ptr<TcpServerPort> port(new TcpServerPort(m_router));
      port->initUnix("@"); // auto bind abstract namespace
      m_path = port->getBoundPath();
      CPPUNIT_ASSERT(m_path != "");
      m_port = port;
   }
   m_users++;

   // create wrapper and initilialize asynchronously
   std::auto_ptr<UnixClientWrapper> conn(new UnixClientWrapper(*this, m_path));
   g.unlock();
   conn->init(south, subDomain);
   return conn.release();
}

void UnixServerWrapper::delConnection()
{
   tsd::common::system::MutexGuard g(m_lock);

   CPPUNIT_ASSERT(m_users > 0);
   m_users--;
   if (m_users == 0) {
      m_port.reset();
   }
}

/*****************************************************************************/

UnixClientWrapper::UnixClientWrapper(UnixServerWrapper &srvWrap, const std::string &serverPath)
   : m_srvWrap(srvWrap)
   , m_serverPath(serverPath)
   , m_client(0)
{
}

UnixClientWrapper::~UnixClientWrapper()
{
   UnixClientWrapper::disconnect();
   m_srvWrap.delConnection();
}

void UnixClientWrapper::init(Router &south, const std::string &subDomain)
{
   std::auto_ptr<TcpClientPort> clientPort(new TcpClientPort(south, subDomain));
   clientPort->initUnix(m_serverPath);

   m_client = clientPort.release();
}

void UnixClientWrapper::disconnect()
{
   if (m_client != 0) {
      m_client->disconnect();
      delete m_client;
      m_client = 0;
   }
}

/*****************************************************************************/

class UnixConnectionTestSuite
   : public TransportSuite
{
   typedef std::map<Router*, UnixServerWrapper*> ServerMap;
   ServerMap m_servers;
   tsd::common::system::Mutex m_lock;

   CPPUNIT_TEST_SUB_SUITE(UnixConnectionTestSuite, TransportSuite);

   // special tests from TransportSuite
   CPPUNIT_TEST(test_bulk_multi);
   CPPUNIT_TEST(test_bulk_connect_disconnect);

   // local tests
   CPPUNIT_TEST(test_connect_regular);
   CPPUNIT_TEST(test_connect_abstract);
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
      UnixServerWrapper *srvWrap;

      {
         // make thread safe
         tsd::common::system::MutexGuard g(m_lock);

         ServerMap::iterator it(m_servers.find(&north));
         if (it == m_servers.end()) {
            m_servers[&north] = srvWrap = new UnixServerWrapper(north);
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
    * Test connection via named socket.
    *
    * The socket must removed after the server has closed.
    */
   void test_connect_regular()
   {
      const std::string path("/tmp/tsd.communication.messageing.test.unix");

      {
         // open server on known path
         Router rootRouter("root");
         TcpServerPort rootServer(rootRouter);
         rootServer.initUnix(path);

         // must be visible
         CPPUNIT_ASSERT(tsd::common::utils::fileExists(path));

         // connect client there
         Router leafRouter("leaf");
         TcpClientPort leafClient(leafRouter);
         leafClient.initUnix(path);
      }

      // make sure socket does not exit anymore
      CPPUNIT_ASSERT(!tsd::common::utils::fileExists(path));
   }

   /**
    * Test connection via abstract namespace socket.
    */
   void test_connect_abstract()
   {
      const std::string path("@loremipsum");

      // open server on known name
      Router rootRouter("root");
      TcpServerPort rootServer(rootRouter);
      rootServer.initUnix(path);
      CPPUNIT_ASSERT_EQUAL(rootServer.getBoundPath(), path);

      // must be not be a file
      CPPUNIT_ASSERT(!tsd::common::utils::fileExists(path));

      // connect client there
      Router leafRouter("leaf");
      TcpClientPort leafClient(leafRouter);
      leafClient.initUnix(path);
   }

   /**
    * Test Unix connection through standard interface.
    *
    * Open a listening socket on an auto bound abstract path and connect the
    * local router to that.
    */
   void test_connect_std_client()
   {
      Router rootRouter("root");

      // open server on ephemeral port / localhost
      TcpServerPort rootServer(rootRouter);
      rootServer.initUnix("@");
      std::string path = rootServer.getBoundPath();
      CPPUNIT_ASSERT(path != "");

      // construct URL and connect
      std::stringstream url;
      url << "unix://" << path;
      std::auto_ptr<IConnection> conn(connectUpstream(url.str()));

      CPPUNIT_ASSERT(conn.get() != NULL);
      conn->disconnect();
   }

   /**
    * Test Unix connection to server established by standard interface.
    *
    * Open a standard listening socket and connect with separate router.
    */
   void test_connect_std_server()
   {
      std::auto_ptr<IConnection> srv(listenDownstream("unix://@tsd.communication.messageing.test.unix"));
      CPPUNIT_ASSERT(srv.get() != NULL);

      Router leafRouter("leaf");
      TcpClientPort leafClient(leafRouter);
      leafClient.initUnix("@tsd.communication.messageing.test.unix");

      srv->disconnect();
   }

   /**
    * Test that connection to invalid server throws exception.
    */
   void test_connect_throws()
   {
      std::auto_ptr<IConnection> conn;

      CPPUNIT_ASSERT_THROW(conn.reset(connectUpstream("unix:///no/such/path")),
         ConnectionException);
   }

};

CPPUNIT_TEST_SUITE_REGISTRATION(UnixConnectionTestSuite);
