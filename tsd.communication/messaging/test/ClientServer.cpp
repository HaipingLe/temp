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
   }

   void init()
   {
      start();
      CPPUNIT_ASSERT(m_startSignal.down(5000));
   }

   void run()
   {
      std::auto_ptr<ILocalIfc> serverIfc(
         m_queue->registerInterface(SampleFactory::getInstance(), "test.server"));

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
         } else if (msg->getReceiverAddr() == serverIfc->getLocalIfcAddr()) {
            StringInd *req = dynamic_cast<StringInd*>(msg.get());
            serverIfc->sendMessage(msg->getSenderAddr(), std::auto_ptr<TsdEvent>(new StringInd(req->m_string)));
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

class ClientServerTestSuite: public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ClientServerTestSuite);

   CPPUNIT_TEST(test_multi_connect);

   CPPUNIT_TEST_SUITE_END();

public:

   void test_multi_connect()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> serverQueue(new Queue("server", rootRouter));
      std::auto_ptr<IQueue> clientQueue(new Queue("client", rootRouter));

      Subscriber server(serverQueue.get());
      server.init();

      std::auto_ptr<IRemoteIfc> clientOne(clientQueue->connectInterface("test.server",
                                          SampleFactory::getInstance(),
                                          3000));
      CPPUNIT_ASSERT(clientOne.get() != NULL);
      std::auto_ptr<IRemoteIfc> clientTwo(clientQueue->connectInterface("test.server",
                                          SampleFactory::getInstance(),
                                          3000));
      CPPUNIT_ASSERT(clientTwo.get() != NULL);
      std::auto_ptr<IRemoteIfc> clientThree(clientQueue->connectInterface("test.server",
                                            SampleFactory::getInstance(),
                                            3000));
      CPPUNIT_ASSERT(clientThree.get() != NULL);

      for (int i = 0; i < 1000; i++) {
         clientOne->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("one")));
         clientTwo->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("two")));
         clientThree->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("three")));

         std::auto_ptr<TsdEvent> m(clientQueue->readMessage(3000));
         StringInd *ind = dynamic_cast<StringInd*>(m.get());
         CPPUNIT_ASSERT(ind != NULL);
         CPPUNIT_ASSERT(ind->m_string == "one");
         CPPUNIT_ASSERT(ind->getReceiverAddr() == clientOne->getLocalIfcAddr());

         m = clientQueue->readMessage(3000);
         ind = dynamic_cast<StringInd*>(m.get());
         CPPUNIT_ASSERT(ind != NULL);
         CPPUNIT_ASSERT(ind->m_string == "two");
         CPPUNIT_ASSERT(ind->getReceiverAddr() == clientTwo->getLocalIfcAddr());

         m = clientQueue->readMessage(3000);
         ind = dynamic_cast<StringInd*>(m.get());
         CPPUNIT_ASSERT(ind != NULL);
         CPPUNIT_ASSERT(ind->m_string == "three");
         CPPUNIT_ASSERT(ind->getReceiverAddr() == clientThree->getLocalIfcAddr());
      }
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(ClientServerTestSuite);
