#include <cstdlib>

#include <tsd/common/system/AtomicInteger.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>

#include "MessageDefs.hpp"
#include "TransportSuite.hpp"

namespace tsd { namespace communication { namespace messaging {

   /**
    * Simple helper class to connect to a single router.
    */
   class TransportSuite::ConnectFactory
   {
      TransportSuite *m_suite;
      Router &m_router;

   public:
      ConnectFactory(TransportSuite *suite, Router &router)
         : m_suite(suite)
         , m_router(router)
      { }

      IConnection* connect(Router &southRouter)
      {
         return m_suite->createConnection(m_router, southRouter);
      }
   };

} } }


using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

namespace {

   class BulkSink
      : public tsd::common::system::Thread
   {
      IQueue *m_queue;
      uint32_t m_remaining;
      bool m_running;

   public:
      BulkSink(const std::string &name, IQueue *queue, uint32_t remaining)
         : tsd::common::system::Thread(name)
         , m_queue(queue)
         , m_remaining(remaining)
         , m_running(false)
      {
      }

      ~BulkSink()
      {
         if (m_running) {
            join();
         }
      }

      void init()
      {
         m_running = true;
         start();
      }

      bool finish()
      {
         if (m_running) {
            m_running = false;
            join();
         }
         return m_remaining == 0;
      }

      void run()
      {
         while (m_remaining > 0) {
            std::auto_ptr<TsdEvent> msg = m_queue->readMessage(10000);
            if (msg.get() == NULL) {
               break;
            }
            m_remaining--;
         }
      }
   };

   class BulkSource
      : public tsd::common::system::Thread
   {
      ILocalIfc *m_localIfc;
      IfcAddr_t m_localIfcDest;
      IRemoteIfc *m_remoteIfc;
      tsd::common::system::AtomicInteger m_remaining;
      bool m_running;

   public:
      BulkSource(const std::string &name, ILocalIfc *ifc, IfcAddr_t dest, int32_t remaining)
         : tsd::common::system::Thread(name)
         , m_localIfc(ifc)
         , m_localIfcDest(dest)
         , m_remoteIfc(NULL)
         , m_remaining(remaining)
         , m_running(false)
      {
      }

      BulkSource(const std::string &name, IRemoteIfc *ifc, int32_t remaining)
         : tsd::common::system::Thread(name)
         , m_localIfc(NULL)
         , m_localIfcDest(0)
         , m_remoteIfc(ifc)
         , m_remaining(remaining)
         , m_running(false)
      {
      }

      ~BulkSource()
      {
         if (m_running) {
            join();
         }
      }

      void init()
      {
         m_running = true;
         start();
      }

      bool finish()
      {
         if (m_running) {
            m_running = false;
            join();
         }
         return m_remaining == -1;
      }

      void shutdown()
      {
         if (m_running) {
            m_running = false;
            m_remaining = 0;
            join();
         }
      }

      void run()
      {
         while (m_remaining.decrement() > 0) {
            if (m_localIfc) {
               if (m_localIfcDest) {
                  m_localIfc->sendMessage(m_localIfcDest,
                     std::auto_ptr<TsdEvent>(new StringInd("asdfqwer")));
               } else {
                  m_localIfc->broadcastMessage(
                     std::auto_ptr<TsdEvent>(new StringInd("asdfqwer")));
               }
            } else {
               m_remoteIfc->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("foobar")));
            }
         }
      }
   };

   class BulkPair {
      std::auto_ptr<IQueue> m_serverQueue;
      std::auto_ptr<ILocalIfc> m_serverIfc;
      std::auto_ptr<IQueue> m_clientQueue;
      std::auto_ptr<IRemoteIfc> m_clientIfc;
      std::auto_ptr<BulkSink> m_clientSink;
      std::auto_ptr<BulkSink> m_serverSink;
      std::auto_ptr<BulkSource> m_clientSource;
      std::auto_ptr<BulkSource> m_serverSource;

   public:
      void init(Router &server, Router &client, const std::string &id, uint32_t messages)
      {
         m_serverQueue.reset(new Queue(id + "-srv", server));
         m_serverIfc.reset(m_serverQueue->registerInterface(SampleFactory::getInstance(), id));

         m_clientQueue.reset(new Queue(id + "-cnt", client));
         m_clientIfc.reset(m_clientQueue->connectInterface(id, SampleFactory::getInstance()));

         // send message to server
         m_clientIfc->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("foobar")));

         // receive message on client
         std::auto_ptr<TsdEvent> msg1 = m_serverQueue->readMessage(3000);
         CPPUNIT_ASSERT(msg1.get() != NULL);
         IfcAddr_t sender = msg1->getSenderAddr();

         m_serverSink.reset(new BulkSink(id + "-ssnk", m_serverQueue.get(), messages));
         m_clientSink.reset(new  BulkSink(id + "-csnk", m_clientQueue.get(), messages));
         m_serverSource.reset(new BulkSource(id + "-ssrc", m_serverIfc.get(), sender, messages));
         m_clientSource.reset(new BulkSource(id + "csrce", m_clientIfc.get(), messages));

         m_serverSource->init();
         m_clientSource->init();
         m_serverSink->init();
         m_clientSink->init();
      }

      void finish()
      {
         CPPUNIT_ASSERT(m_clientSource->finish());
         CPPUNIT_ASSERT(m_serverSource->finish());
         CPPUNIT_ASSERT(m_clientSink->finish());
         CPPUNIT_ASSERT(m_serverSink->finish());
      }
   };


   class RepeatedConnector
      : public tsd::common::system::Thread
   {
      tsd::communication::messaging::TransportSuite::ConnectFactory &m_factory;
      uint32_t m_remaining;
      bool m_running;

   public:
      RepeatedConnector(const std::string &name,
            tsd::communication::messaging::TransportSuite::ConnectFactory &factory,
            uint32_t remaining)
         : tsd::common::system::Thread(name)
         , m_factory(factory)
         , m_remaining(remaining)
         , m_running(false)
      { }

      ~RepeatedConnector()
      {
         if (m_running) {
            join();
         }
      }

      void init()
      {
         m_running = true;
         start();
      }

      bool finish()
      {
         if (m_running) {
            m_running = false;
            join();
         }
         return m_remaining == 0;
      }

      void run()
      {
         try {
            while (m_remaining > 0) {
               Router leafRouter("leaf");
               std::auto_ptr<IConnection> connection(m_factory.connect(leafRouter));

               std::auto_ptr<IQueue> clientQueue(new Queue("client", leafRouter));
               std::auto_ptr<IRemoteIfc> clientIfc(clientQueue->connectInterface(
                  "beacon", SampleFactory::getInstance()));
               CPPUNIT_ASSERT(clientIfc.get() != NULL);
               clientIfc->subscribe(STRING_IND);

               tsd::common::system::Thread::sleep(std::rand() % 3);
               m_remaining--;
            }
         } catch (std::exception &e) {
            std::cerr << "RepeatedConnector failed: " << e.what()
                      << "; " << m_remaining << " iterations remaining"
                      << std::endl;
         }
      }
   };
}


/*****************************************************************************/

TransportSuite::~TransportSuite()
{ }

/**
 * Test a connection and message exchange.
 *
 * Send a message in both directions.
 */
void TransportSuite::test_connect_stub()
{
   Router rootRouter("root");
   Router leafRouter("leaf");

   std::auto_ptr<IConnection> root2leafConnection(createConnection(rootRouter, leafRouter));

   std::auto_ptr<IQueue> q1(new Queue("server", rootRouter));
   std::auto_ptr<ILocalIfc> serverIfc(
      q1->registerInterface(SampleFactory::getInstance(), "test.bcast"));

   std::auto_ptr<IQueue> q2(new Queue("client", leafRouter));
   std::auto_ptr<IRemoteIfc> clientIfc(
      q2->connectInterface("test.bcast", SampleFactory::getInstance()));

   // send message to server
   clientIfc->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("foobar")));

   // receive message and send back
   std::auto_ptr<TsdEvent> msg1 = q1->readMessage(3000);
   CPPUNIT_ASSERT(msg1.get() != NULL);
   IfcAddr_t sender = msg1->getSenderAddr();
   serverIfc->sendMessage(sender, msg1);

   // receive message at client
   std::auto_ptr<TsdEvent> msg2 = q2->readMessage(3000);
   StringInd *si = dynamic_cast<StringInd*>(msg2.get());
   CPPUNIT_ASSERT(si != NULL);
   CPPUNIT_ASSERT(si->m_string == "foobar");
}

/**
 * Test a connection and message exchange.
 *
 * Send a message in both directions. Allocates a subdomain on the south router.
 */
void TransportSuite::test_connect_auth()
{
   Router rootRouter("root");
   Router leafRouter("leaf");

   std::auto_ptr<IConnection> root2leafConnection(createConnection(rootRouter, leafRouter, "qwertz"));

   std::auto_ptr<IQueue> q1(new Queue("server", leafRouter));
   std::auto_ptr<ILocalIfc> serverIfc(
      q1->registerInterface(SampleFactory::getInstance(), "test.bcast"));

   std::auto_ptr<IQueue> q2(new Queue("client", rootRouter));
   std::auto_ptr<IRemoteIfc> clientIfc(
      q2->connectInterface("/qwertz/test.bcast", SampleFactory::getInstance()));

   // send message to server
   clientIfc->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("foobar")));

   // receive message and send back
   std::auto_ptr<TsdEvent> msg1 = q1->readMessage(3000);
   CPPUNIT_ASSERT(msg1.get() != NULL);
   IfcAddr_t sender = msg1->getSenderAddr();
   serverIfc->sendMessage(sender, msg1);

   // receive message at client
   std::auto_ptr<TsdEvent> msg2 = q2->readMessage(3000);
   StringInd *si = dynamic_cast<StringInd*>(msg2.get());
   CPPUNIT_ASSERT(si != NULL);
   CPPUNIT_ASSERT(si->m_string == "foobar");
}

/**
 * Test bulk message transfer.
 *
 * Connect two clients and send 100000 messages to each side as fast as
 * possible.
 */
void TransportSuite::test_bulk_single()
{
   Router rootRouter("root");
   Router leafRouter("leaf");

   // connect routers
   std::auto_ptr<IConnection> root2leafConnection(createConnection(rootRouter, leafRouter));

   // start message flow
   BulkPair pair;
   pair.init(rootRouter, leafRouter, "x", 100000);

   // wait for end
   pair.finish();
}

/**
 * Test single huge ping-pong message.
 *
 * Transfer one giant message in each direction.
 */
void TransportSuite::test_ping_huge()
{
   Router rootRouter("root");
   Router leafRouter("leaf");

   // connect routers
   std::auto_ptr<IConnection> root2leafConnection(createConnection(rootRouter, leafRouter));


   // connect interfaces
   std::auto_ptr<IQueue> serverQueue(new Queue("srv", rootRouter));
   std::auto_ptr<ILocalIfc> serverIfc(serverQueue->registerInterface(SampleFactory::getInstance(), "ping"));

   std::auto_ptr<IQueue> clientQueue(new Queue("cnt", leafRouter));
   std::auto_ptr<IRemoteIfc> clientIfc(clientQueue->connectInterface("ping", SampleFactory::getInstance()));

   std::string payload;
   payload.resize(256*1024, 'j');

   // send message to server
   clientIfc->sendMessage(std::auto_ptr<TsdEvent>(new StringInd(payload)));

   // receive message on server
   std::auto_ptr<TsdEvent> msg1 = serverQueue->readMessage(3000);
   CPPUNIT_ASSERT(msg1.get() != NULL);
   StringInd *realMsg1 = dynamic_cast<StringInd*>(msg1.get());
   CPPUNIT_ASSERT(realMsg1 != NULL);
   CPPUNIT_ASSERT(realMsg1->m_string == payload);
   IfcAddr_t sender = msg1->getSenderAddr();

   // send message back to client
   serverIfc->sendMessage(sender, std::auto_ptr<TsdEvent>(new StringInd(payload)));

   // receive message on client
   std::auto_ptr<TsdEvent> msg2 = clientQueue->readMessage(3000);
   CPPUNIT_ASSERT(msg2.get() != NULL);
   StringInd *realMsg2 = dynamic_cast<StringInd*>(msg2.get());
   CPPUNIT_ASSERT(realMsg2 != NULL);
   CPPUNIT_ASSERT(realMsg2->m_string == payload);
}

namespace {

}

/**
 * Test bulk message transfer between multiple routers.
 *
 * Connect three pairs of clients and send 100000 messages to each side as fast
 * as possible. Every client is on a different router with a common "root"
 * router.
 */
void TransportSuite::test_bulk_multi()
{
   Router rootRouter("root");
   Router leafA1Router("leafA1");
   Router leafA2Router("leafA2");
   Router leafB1Router("leafB1");
   Router leafB2Router("leafB2");
   Router leafC1Router("leafC1");
   Router leafC2Router("leafC2");

   // connect all routers
   std::auto_ptr<IConnection> a1Connection(createConnection(rootRouter, leafA1Router));
   std::auto_ptr<IConnection> a2Connection(createConnection(rootRouter, leafA2Router));
   std::auto_ptr<IConnection> b1Connection(createConnection(rootRouter, leafB1Router));
   std::auto_ptr<IConnection> b2Connection(createConnection(rootRouter, leafB2Router));
   std::auto_ptr<IConnection> c1Connection(createConnection(rootRouter, leafC1Router));
   std::auto_ptr<IConnection> c2Connection(createConnection(rootRouter, leafC2Router));

   // create three pairs
   const uint32_t MESSAGES = 1000;

   BulkPair pairA;
   pairA.init(leafA1Router, leafA2Router, "a", MESSAGES);
   BulkPair pairB;
   pairB.init(leafB1Router, leafB2Router, "b", MESSAGES);
   BulkPair pairC;
   pairC.init(leafC1Router, leafC2Router, "c", MESSAGES);

   // wait for storm to end
   pairA.finish();
   pairB.finish();
   pairC.finish();
}

/**
 * Test connect/disconnect in parallel.
 *
 * Spawn 4 threads that each do a connect/disconnect in parallel to the server.
 * While doing that we continously send messages to the clients so that the
 * code *will* see defunct ports during read, write and from the event loop.
 */
void TransportSuite::test_bulk_connect_disconnect()
{
   Router rootRouter("root");
   ConnectFactory factory(this, rootRouter);

   std::auto_ptr<IQueue> serverQueue(new Queue("beacon", rootRouter));
   std::auto_ptr<ILocalIfc> serverIfc(
      serverQueue->registerInterface(SampleFactory::getInstance(), "beacon"));

   BulkSource beacon("beacon", serverIfc.get(), 0, 0xfffffff);

   RepeatedConnector c1("c1", factory, 100);
   RepeatedConnector c2("c2", factory, 100);
   RepeatedConnector c3("c3", factory, 100);
   RepeatedConnector c4("c4", factory, 100);

   beacon.init();
   c1.init();
   c2.init();
   c3.init();
   c4.init();

   CPPUNIT_ASSERT(c1.finish());
   CPPUNIT_ASSERT(c2.finish());
   CPPUNIT_ASSERT(c3.finish());
   CPPUNIT_ASSERT(c4.finish());

   beacon.shutdown();
}
