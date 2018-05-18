
#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>
#include <cstdlib>

#include <tsd/common/system/Thread.hpp>
#include <tsd/common/utils/DuplexShm.hpp>
#include <tsd/communication/messaging/Connection.hpp>

// internal API
#include <tsd/communication/messaging/ConnectionImpl.hpp>

#include "TransportSuite.hpp"
#include "MessageDefs.hpp"

namespace tsd { namespace communication { namespace messaging {

   class Router;

   class ShmConnectionHalf
      : public tsd::common::utils::DuplexShm
      , public IConnectionCallbacks
   {
      tsd::common::system::Semaphore &m_wakeMe;
      tsd::common::system::Semaphore &m_wakeThem;

   protected:
      ConnectionImpl *m_connection;

      ShmConnectionHalf(Router &router,
                        tsd::common::system::Semaphore &wakeMe,
                        tsd::common::system::Semaphore &wakeThem)
         : m_wakeMe(wakeMe)
         , m_wakeThem(wakeThem)
      {
         m_connection = new ConnectionImpl(*this, router);
      }

      ~ShmConnectionHalf()
      {
         m_connection->disconnect();
         delete m_connection;
      }

      // tsd::common::utils::DuplexShm
      void wakeRemote()
      {
         m_wakeThem.up();
      }

      // tsd::common::utils::DuplexShm
      bool waitForRemote(uint32_t timeout)
      {
         return m_wakeMe.down(timeout);
      }

      // tsd::common::utils::DuplexShm
      void processData(const void* data, uint32_t size)
      {
         m_connection->processData(data, size);
      }

      // tsd::common::utils::DuplexShm
      bool getData(const void*& data, uint32_t& size)
      {
         size_t s;
         bool ret = m_connection->getData(data, s);
         size = static_cast<uint32_t>(s);
         return ret;
      }

      // tsd::common::utils::DuplexShm
      void consumeData(const uint32_t amount)
      {
         m_connection->consumeData(amount);
      }

      // IConnectionCallbacks
      void wakeup()
      {
         m_wakeMe.up();
      }

   public:
      void disconnect()
      {
         m_connection->disconnect();
      }
   };

   class ShmConnection
      : public IConnection
   {
      class Listener
         : public ShmConnectionHalf
         , protected tsd::common::system::Thread
      {
         void *m_shmPtr;
         size_t m_shmLen;

      protected:
         void run(); // tsd::common::system::Thread
         bool setupConnection(); // IConnectionCallbacks
         void teardownConnection(); // IConnectionCallbacks

      public:
         Listener(Router &router,
                  tsd::common::system::Semaphore &wakeMe,
                  tsd::common::system::Semaphore &wakeThem,
                  void *shmPtr, size_t shmLen);
         ~Listener();
         bool init();
      };

      class Connector
         : public ShmConnectionHalf
         , protected tsd::common::system::Thread
      {
         void *m_shmPtr;
         size_t m_shmLen;

      protected:
         void run(); // tsd::common::system::Thread
         bool setupConnection(); // IConnectionCallbacks
         void teardownConnection(); // IConnectionCallbacks

      public:
         Connector(Router &router,
                  tsd::common::system::Semaphore &wakeMe,
                  tsd::common::system::Semaphore &wakeThem,
                  void *shmPtr, size_t shmLen);
         ~Connector();
         bool init(const std::string &subDomain);
      };

      tsd::common::system::Semaphore m_listenerWake;
      tsd::common::system::Semaphore m_connectorWake;
      void *m_shmPtr;
      size_t m_shmLen;
      Listener *m_listener;
      Connector *m_connector;

   public:
      ShmConnection(size_t shmLen = 4096);
      ~ShmConnection();

      bool connect(Router &upstreamRouter, Router &downstreamRouter,
                   const std::string &subDomain = "");

      void disconnect(); // IConnection
   };

} } }

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

/*****************************************************************************/

ShmConnection::Listener::Listener(Router &router,
         tsd::common::system::Semaphore &wakeMe,
         tsd::common::system::Semaphore &wakeThem,
         void *shmPtr, size_t shmLen)
   : ShmConnectionHalf(router, wakeMe, wakeThem)
   , tsd::common::system::Thread("ShmConn::Listener")
   , m_shmPtr(shmPtr)
   , m_shmLen(shmLen)
{ }

ShmConnection::Listener::~Listener()
{ }

bool ShmConnection::Listener::init()
{
   return m_connection->listenDownstream();
}

void ShmConnection::Listener::run()
{
   bool ok;

   do {
      ok = waitForConnect();
      if (ok) {
         m_connection->setConnected();
         ok = operate();
         m_connection->setDisconnected();
      }
   } while (ok);
}

bool ShmConnection::Listener::setupConnection()
{
   if (!initListen(m_shmPtr, static_cast<uint32_t>(m_shmLen))) {
      return false;
   }

   start();
   return true;
}

void ShmConnection::Listener::teardownConnection()
{
   finish();
   wakeup();
   join();
}

/*****************************************************************************/

ShmConnection::Connector::Connector(Router &router,
         tsd::common::system::Semaphore &wakeMe,
         tsd::common::system::Semaphore &wakeThem,
         void *shmPtr, size_t shmLen)
   : ShmConnectionHalf(router, wakeMe, wakeThem)
   , tsd::common::system::Thread("ShmConn::Connector")
   , m_shmPtr(shmPtr)
   , m_shmLen(shmLen)
{ }

ShmConnection::Connector::~Connector()
{ }

bool ShmConnection::Connector::init(const std::string &subDomain)
{
   return m_connection->connectUpstream(subDomain);
}

void ShmConnection::Connector::run()
{
   bool ok;

   ok = connectRemote();
   if (ok) {
      m_connection->setConnected();
      ok = operate();
      m_connection->setDisconnected();
   }
}

bool ShmConnection::Connector::setupConnection()
{
   if (!initConnect(m_shmPtr, static_cast<uint32_t>(m_shmLen))) {
      return false;
   }

   start();
   return true;
}

void ShmConnection::Connector::teardownConnection()
{
   finish();
   wakeup();
   join();
}

/*****************************************************************************/

ShmConnection::ShmConnection(size_t shmLen)
   : m_listenerWake(0)
   , m_connectorWake(0)
   , m_shmPtr(std::malloc(shmLen))
   , m_shmLen(shmLen)
   , m_listener(0)
   , m_connector(0)
{
}

ShmConnection::~ShmConnection()
{
   ShmConnection::disconnect();

   if (m_shmPtr) {
      std::free(m_shmPtr);
   }
}

bool ShmConnection::connect(Router &upstreamRouter, Router &downstreamRouter,
                            const std::string &subDomain)
{
   if (m_listener != 0 || m_connector != 0) {
      return false;
   }

   std::auto_ptr<Listener> listener(new Listener(upstreamRouter, m_listenerWake,
      m_connectorWake, m_shmPtr, m_shmLen));

   if (!listener->init()) {
      return false;
   }

   std::auto_ptr<Connector> connector(new Connector(downstreamRouter, m_connectorWake,
      m_listenerWake, m_shmPtr, m_shmLen));

   if (!connector->init(subDomain)) {
      listener->disconnect();
      return false;
   }

   m_listener = listener.release();
   m_connector = connector.release();
   return true;
}

void ShmConnection::disconnect()
{
   if (m_connector != 0) {
      m_connector->disconnect();
      delete m_connector;
      m_connector = 0;
   }

   if (m_listener != 0) {
      m_listener->disconnect();
      delete m_listener;
      m_listener = 0;
   }
}

/*****************************************************************************/

class ShmConnectionTestSuite
   : public TransportSuite
{
   CPPUNIT_TEST_SUB_SUITE(ShmConnectionTestSuite, TransportSuite);

   // special tests from TransportSuite
   CPPUNIT_TEST(test_bulk_multi);

   // no additional local tests so far

   CPPUNIT_TEST_SUITE_END();

protected:
   // TransportSuite
   IConnection* createConnection(Router &north, Router &south, const std::string &subDomain)
   {
      std::auto_ptr<ShmConnection> conn(new ShmConnection);
      if (conn->connect(north, south, subDomain)) {
         return conn.release();
      } else {
         return NULL;
      }
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ShmConnectionTestSuite);
