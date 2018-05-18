#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>

#include <tsd/common/config/ConfigManager.hpp>
#include <tsd/common/logging/LoggingManager.hpp>
#include <tsd/common/system/Clock.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/Queue.hpp>

// internal API
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>

#include "MessageDefs.hpp"

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

class TimerWatcher
   : public tsd::common::system::Thread
{
   IQueue *m_queue;
   tsd::common::system::Semaphore m_sem;
   tsd::common::system::Semaphore m_startSignal;

public:
   TimerWatcher(IQueue *queue)
      : tsd::common::system::Thread("listener")
      , m_queue(queue)
      , m_sem(0)
      , m_startSignal(0)
   {
   }

   ~TimerWatcher()
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
               case TIMER_IND:
                  m_sem.up();
                  break;
               default:
                  std::cout << "Unknown message on loopback iterface: " << msg->getEventId()
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

class TimersTestSuite: public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(TimersTestSuite);

   CPPUNIT_TEST(test_single_shot);
   CPPUNIT_TEST(test_single_shot_timing);
   CPPUNIT_TEST(test_repeating_wait);
   CPPUNIT_TEST(test_repeating_burst);
   CPPUNIT_TEST(test_repeating_order);
   CPPUNIT_TEST(test_cancel_active);
   CPPUNIT_TEST(test_cancel_expired);
   CPPUNIT_TEST(test_cancel_expired_repeating);

   CPPUNIT_TEST(test_threaded_single_shot);

   CPPUNIT_TEST_SUITE_END();

public:
   /**
    * Test a single shot timer.
    *
    * Also test that receiver address is loopback address and that the right
    * event was received.
    */
   void test_single_shot()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      std::auto_ptr<TsdEvent> timer(new TimerInd);
      timer->setSenderAddr(1);
      timer->setReceiverAddr(2);

      q->startTimer(timer, 50, false);
      std::auto_ptr<TsdEvent> msg = q->readMessage(1000);

      CPPUNIT_ASSERT(msg.get() != NULL);
      CPPUNIT_ASSERT_EQUAL(msg->getEventId(), TIMER_IND);
      CPPUNIT_ASSERT_EQUAL(msg->getSenderAddr(), IfcAddr_t(1));
      CPPUNIT_ASSERT_EQUAL(msg->getReceiverAddr(), LOOPBACK_ADDRESS);
   }

   /**
    * Test that timer blocks readMessage() as long as expected.
    */
   void test_single_shot_timing()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      uint32_t timeStart = tsd::common::system::Clock::getTickCounter();
      q->startTimer(std::auto_ptr<TsdEvent>(new TimerInd), 200, false);
      std::auto_ptr<TsdEvent> msg = q->readMessage(1000);
      uint32_t timeEnd = tsd::common::system::Clock::getTickCounter();

      CPPUNIT_ASSERT(msg.get() != NULL);
      CPPUNIT_ASSERT((timeEnd - timeStart) >= 100);
   }

   /**
    * Test a repeating timer.
    *
    * Waits the whole time on the queue. The real time spent waiting must be
    * correct (time * nr).
    */
   void test_repeating_wait()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      uint32_t timeStart = tsd::common::system::Clock::getTickCounter();

      q->startTimer(std::auto_ptr<TsdEvent>(new TimerInd), 20, true);
      for (int i=0; i<3; i++) {
         std::auto_ptr<TsdEvent> msg = q->readMessage(1000);
         CPPUNIT_ASSERT(msg.get() != NULL);
      }

      uint32_t timeEnd = tsd::common::system::Clock::getTickCounter();

      CPPUNIT_ASSERT((timeEnd - timeStart) >= 60);
      CPPUNIT_ASSERT((timeEnd - timeStart) < 1000);
   }

   /**
    * Test a repeating timer.
    *
    * The thread sleeps for some time. After that the correct number of timer
    * messages must be received in a burst.
    */
   void test_repeating_burst()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      q->startTimer(std::auto_ptr<TsdEvent>(new TimerInd), 20, true);
      tsd::common::system::Thread::sleep(100);
      std::auto_ptr<TsdEvent> msg;
      int i = -1;
      do {
         msg = q->readMessage(0);
         i++;
      } while (msg.get() != 0);

      CPPUNIT_ASSERT(i >= 5);
   }

   /**
    * Test two repeating timers and check that order is as expected.
    *
    * The two timers are expected to interleave.
    *
    * |- 0      |- 10     |- 20
    * v         v         v
    *    1  1  1  1  1  1  1  1  1  1
    *        2      2      2      2
    */
   void test_repeating_order()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      q->startTimer(std::auto_ptr<TsdEvent>(new TimerInd(1)), 3, true);
      q->startTimer(std::auto_ptr<TsdEvent>(new TimerInd(2)), 7, true);

      tsd::common::system::Thread::sleep(30);

      std::auto_ptr<TimerInd> msg;
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);

      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 2);

      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);

      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 2);

      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);

      // undefined order at 21ms
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get());
      uint32_t prevId = msg->m_id;
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id != prevId);

      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);
      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 1);

      msg.reset(dynamic_cast<TimerInd*>(q->readMessage(0).release()));
      CPPUNIT_ASSERT(msg.get() && msg->m_id == 2);
   }

   /**
    * Test to cancel active timer.
    */
   void test_cancel_active()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      TimerRef_t timer = q->startTimer(
         std::auto_ptr<TsdEvent>(new TimerInd), 20000, false);
      tsd::common::system::Thread::sleep(10);

      bool active = q->stopTimer(timer);
      CPPUNIT_ASSERT(active);
   }

   /**
    * Test to cancel expired single-shot timer.
    *
    * The timer message must not be received.
    */
   void test_cancel_expired()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      TimerRef_t timer = q->startTimer(
         std::auto_ptr<TsdEvent>(new TimerInd), 20, false);
      tsd::common::system::Thread::sleep(100);

      bool active = q->stopTimer(timer);
      CPPUNIT_ASSERT(!active);
      std::auto_ptr<TsdEvent> msg(q->readMessage(0));
      CPPUNIT_ASSERT(msg.get() == NULL);
   }

   /**
    * Test to cancel expired repeating timer.
    *
    * No timer messages must be received.
    */
   void test_cancel_expired_repeating()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      TimerRef_t timer = q->startTimer(
         std::auto_ptr<TsdEvent>(new TimerInd), 20, true);
      tsd::common::system::Thread::sleep(60);

      bool active = q->stopTimer(timer);
      CPPUNIT_ASSERT(active);
      std::auto_ptr<TsdEvent> msg(q->readMessage(0));
      CPPUNIT_ASSERT(msg.get() == NULL);
   }

   /**
    * Test a single shot timer across threads.
    *
    * A timer is started from one thread while another Thread is in a blocking
    * readMessage(). The blocked thread must receive the timer message.
    */
   void test_threaded_single_shot()
   {
      Router rootRouter("root");
      std::auto_ptr<IQueue> q(new Queue("clock", rootRouter));

      TimerWatcher watcher(q.get());
      watcher.init();
      // make sure that other thread is in readMessage()
      tsd::common::system::Thread::sleep(100);

      TimerRef_t timer = q->startTimer(std::auto_ptr<TsdEvent>(new TimerInd),
         20, false);

      CPPUNIT_ASSERT(watcher.wait());
      CPPUNIT_ASSERT(!q->stopTimer(timer));
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TimersTestSuite);

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
