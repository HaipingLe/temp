#include <iostream>
#include <iomanip>

#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/common/config/ConfigManager.hpp>

#include <tsd/communication/messaging/Queue.hpp>

// internal API
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>

#include "InternalPort.hpp"
#include "MessageDefs.hpp"

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

class Subscriber
   : public tsd::common::system::Thread
{
   IQueue *m_queue;
   tsd::common::system::Semaphore m_sem;
   tsd::common::system::Semaphore m_startSignal;

public:
   Subscriber(IQueue *queue)
      : tsd::common::system::Thread("listener")
      , m_queue(queue)
      , m_sem(0)
      , m_startSignal(0)
   {
   }

   ~Subscriber()
   {
      if (hasRun()) {
         m_queue->sendSelfMessage(std::auto_ptr<TsdEvent>(new QuitInd));
         join();
      }

      delete m_queue;
   }

   void init()
   {
      start();
      CPPUNIT_ASSERT(m_startSignal.down(5000));
   }

   void run()
   {
      std::auto_ptr<IRemoteIfc> m_tunerIfc(m_queue->connectInterface(
            "test.bcast", SampleFactory::getInstance()));
      m_tunerIfc->subscribe(DUMMY_IND);

      bool alive = true;
      m_startSignal.up();

      while (alive) {
         std::auto_ptr<TsdEvent> msg = m_queue->readMessage();

         // provide dispatch class for the following code
         if (msg->getReceiverAddr() == LOOPBACK_ADDRESS) {
            switch (msg->getEventId()) {
               case QUIT_IND:
                  alive = false;
                  break;
               default:
                  std::cout << "Unknown message on loopback iterface: " << msg->getEventId()
                            << &std::endl;
                  break;
            }
         } else if (msg->getReceiverAddr() == m_tunerIfc->getLocalIfcAddr()) {
            switch (msg->getEventId()) {
               case DUMMY_IND:
               {
                  m_sem.up();
                  break;
               }
               default:
                  std::cout << "Unknown message on tuner iterface: " << msg->getEventId()
                            << &std::endl;
                  break;
            }
         } else {
            std::cout << "Unknown message received" << &std::endl;
         }
      }
   }

   bool wait(uint32_t timeout = 1000)
   {
      return m_sem.down(timeout);
   }
};

class PubSubTestSuite: public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(PubSubTestSuite);

   CPPUNIT_TEST(test_local);
   CPPUNIT_TEST(test_downstream);
   CPPUNIT_TEST(test_upstream);

   CPPUNIT_TEST_SUITE_END();

   public:
      /**
       * Test subscriber and publisher on the same router.
       */
      void test_local()
      {
         std::auto_ptr<IQueue> q(createQueue("publisher"));
         std::auto_ptr<ILocalIfc> statusIfc(
            q->registerInterface(SampleFactory::getInstance(), "test.bcast"));

         Subscriber subscriber(createQueue("listener"));
         subscriber.init();

         // send multicast message to subscribers
         statusIfc->broadcastMessage(std::auto_ptr<TsdEvent>(new DummyInd));
         CPPUNIT_ASSERT(subscriber.wait());
      }

      /**
       * Test downstream subscriber with upstream publisher.
       */
      void test_downstream()
      {
         Router rootRouter("root");

         Router midRouter("mid");
         InternalPort portRoot2Mid;
         portRoot2Mid.connect(rootRouter, midRouter);

         Router leafRouter("leaf");
         InternalPort portMid2Leaf;
         portMid2Leaf.connect(midRouter, leafRouter);

         std::auto_ptr<IQueue> q(new Queue("publisher", rootRouter));
         std::auto_ptr<ILocalIfc> statusIfc(
            q->registerInterface(SampleFactory::getInstance(), "test.bcast"));

         Subscriber subscriber(new Queue("listener", leafRouter));
         subscriber.init();

         // wait for quiet state
         bool idle;
         do {
            idle = portRoot2Mid.isIdle() && portMid2Leaf.isIdle();
         } while (!idle);

         // send multicast message to subscribers
         statusIfc->broadcastMessage(std::auto_ptr<TsdEvent>(new DummyInd));
         CPPUNIT_ASSERT(subscriber.wait());
      }

      /**
       * Test upstream subscriber with downstream publisher.
       */
      void test_upstream()
      {
         Router rootRouter("root");

         Router midRouter("mid");
         InternalPort portRoot2Mid;
         portRoot2Mid.connect(rootRouter, midRouter);

         Router leafRouter("leaf");
         InternalPort portMid2Leaf;
         portMid2Leaf.connect(midRouter, leafRouter);

         std::auto_ptr<IQueue> q(new Queue("publisher", leafRouter));
         std::auto_ptr<ILocalIfc> statusIfc(
            q->registerInterface(SampleFactory::getInstance(), "test.bcast"));

         Subscriber subscriber(new Queue("listener", rootRouter));
         subscriber.init();

         // wait for quiet state
         bool idle;
         do {
            idle = portRoot2Mid.isIdle() && portMid2Leaf.isIdle();
         } while (!idle);

         // send multicast message to subscribers
         statusIfc->broadcastMessage(std::auto_ptr<TsdEvent>(new DummyInd));
         CPPUNIT_ASSERT(subscriber.wait());
      }
};

CPPUNIT_TEST_SUITE_REGISTRATION(PubSubTestSuite);



class Cleanup
{
public:
   ~Cleanup()
   {
      tsd::common::logging::LoggingManager::cleanup();
      tsd::common::system::Thread::cleanup();
      tsd::common::config::ConfigManager::cleanup();
   }
};

static Cleanup cleanup;
