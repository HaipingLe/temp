#include <iostream>
#include <iomanip>

#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>
#include <tsd/common/system/Clock.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>

#include <tsd/communication/messaging/Queue.hpp>

// internal API
#include <tsd/communication/messaging/QueueInternal.hpp>
#include <tsd/communication/messaging/Router.hpp>

#include "InternalPort.hpp"
#include "MessageDefs.hpp"

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

class AsyncRegister
   : protected tsd::common::system::Thread
{
   IQueue *m_queue;
   std::string m_name;
   uint32_t m_delay;
   tsd::common::system::Semaphore m_finish;

protected:
   void run()
   {
      tsd::common::system::Thread::sleep(m_delay);
      std::auto_ptr<ILocalIfc> pubIfc(
         m_queue->registerInterface(SampleFactory::getInstance(), m_name));
      m_finish.down();
   }

public:
   AsyncRegister(IQueue *queue, const std::string &name, uint32_t delay)
      : tsd::common::system::Thread("AsyncRegister")
      , m_queue(queue)
      , m_name(name)
      , m_delay(delay)
      , m_finish(0)
   {
      start();
   }

   ~AsyncRegister()
   {
      m_finish.up();
      join();
   }
};

class NameServerTestSuite: public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(NameServerTestSuite);

   CPPUNIT_TEST(test_local);
   CPPUNIT_TEST(test_local_timeout);
   CPPUNIT_TEST(test_local_delayed);
   CPPUNIT_TEST(test_stub);
   CPPUNIT_TEST(test_stub_deep);
   CPPUNIT_TEST(test_stub_timeout);
   CPPUNIT_TEST(test_stub_delayed_root);
   CPPUNIT_TEST(test_stub_delayed_leaf);
   CPPUNIT_TEST(test_auth_down_search);
   CPPUNIT_TEST(test_auth_up_search);
   CPPUNIT_TEST(test_auth_tree_search);
   CPPUNIT_TEST(test_auth_tree_timout);
   CPPUNIT_TEST(test_disconnect_stub_1);

   // FIXME: broken tests
   //CPPUNIT_TEST(test_disconnect_stub_2);

   CPPUNIT_TEST_SUITE_END();

   bool checkIfcExist(const std::string &name, IQueue *q,
                      uint32_t timeout = 10000)
   {
      std::auto_ptr<IRemoteIfc> ifc(
         q->connectInterface(name, SampleFactory::getInstance(), timeout));

      return ifc.get() != NULL;
   }

public:
   void test_local()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("publisher", rootRouter));
      std::auto_ptr<IQueue> subQ(new Queue("subscriver", rootRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "test"));

      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface("test", SampleFactory::getInstance()));

      CPPUNIT_ASSERT_EQUAL(pubIfc->getLocalIfcAddr(), subIfc->getRemoteIfcAddr());
   }

   void test_local_timeout()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> subQ(new Queue("subscriver", rootRouter));

      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface("test", SampleFactory::getInstance(), 100));

      CPPUNIT_ASSERT(subIfc.get() == NULL);
   }

   void test_local_delayed()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("publisher", rootRouter));
      std::auto_ptr<IQueue> subQ(new Queue("subscriver", rootRouter));

      uint32_t startTime = tsd::common::system::Clock::getTickCounter();
      AsyncRegister asyncReg(pubQ.get(), "test", 500);
      bool found = checkIfcExist("test", subQ.get());
      uint32_t endTime = tsd::common::system::Clock::getTickCounter();

      CPPUNIT_ASSERT(found);
      CPPUNIT_ASSERT((endTime - startTime) >= 500);
   }

   void test_stub()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      CPPUNIT_ASSERT(checkIfcExist("foo", rootQ.get()));
      CPPUNIT_ASSERT(checkIfcExist("foo", leafQ.get()));
   }

   void test_stub_deep()
   {
      Router rootRouter("root");
      Router midRouter("mid");
      InternalPort portRoot2Mid;
      CPPUNIT_ASSERT(portRoot2Mid.connect(rootRouter, midRouter));
      Router leafRouter("leaf");
      InternalPort portMid2Leaf;
      CPPUNIT_ASSERT(portMid2Leaf.connect(midRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> midQ(new Queue("subscriver", midRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      CPPUNIT_ASSERT(checkIfcExist("foo", rootQ.get()));
      CPPUNIT_ASSERT(checkIfcExist("foo", midQ.get()));
      CPPUNIT_ASSERT(checkIfcExist("foo", leafQ.get()));
   }

   void test_stub_timeout()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));
      CPPUNIT_ASSERT(!checkIfcExist("test", leafQ.get(), 100));
   }

   void test_stub_delayed_root()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      uint32_t startTime = tsd::common::system::Clock::getTickCounter();
      AsyncRegister asyncReg(leafQ.get(), "test", 500);
      bool found = checkIfcExist("test", rootQ.get());
      uint32_t endTime = tsd::common::system::Clock::getTickCounter();

      CPPUNIT_ASSERT(found);
      CPPUNIT_ASSERT((endTime - startTime) >= 500);
   }

   void test_stub_delayed_leaf()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      uint32_t startTime = tsd::common::system::Clock::getTickCounter();
      AsyncRegister asyncReg(leafQ.get(), "test", 500);
      bool found = checkIfcExist("test", leafQ.get());
      uint32_t endTime = tsd::common::system::Clock::getTickCounter();

      CPPUNIT_ASSERT(found);
      CPPUNIT_ASSERT((endTime - startTime) >= 500);
   }

   void test_auth_down_search()
   {
      Router rootRouter("root");

      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;

      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter, "leaf"));

      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));
      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      // search downwards
      std::auto_ptr<IRemoteIfc> ifc1(
         rootQ->connectInterface("/leaf/foo", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc1.get() != NULL);

      // make sure the interface is not registered on the root
      std::auto_ptr<IRemoteIfc> ifc2(
         rootQ->connectInterface("foo", SampleFactory::getInstance(), 100));
      CPPUNIT_ASSERT(ifc2.get() == NULL);

      // plain interface name is valid on registerd router
      std::auto_ptr<IRemoteIfc> ifc3(
         leafQ->connectInterface("foo", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc3.get() != NULL);

      // fully qualified interface name is valid on registerd router
      std::auto_ptr<IRemoteIfc> ifc4(
         leafQ->connectInterface("/leaf/foo", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc4.get() != NULL);

      // the router domain is no valid interface
      std::auto_ptr<IRemoteIfc> ifc5(
         leafQ->connectInterface("/leaf", SampleFactory::getInstance(), 300));
      CPPUNIT_ASSERT(ifc5.get() == NULL);
   }

   void test_auth_up_search()
   {
      Router rootRouter("root");

      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;

      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter, "leaf"));

      std::auto_ptr<IQueue> rootQ(new Queue("root-queue", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("leaf-queue", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         rootQ->registerInterface(SampleFactory::getInstance(), "bar"));

      // search upwards
      std::auto_ptr<IRemoteIfc> ifc1(
         leafQ->connectInterface("/bar", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc1.get() != NULL);

      // make sure the interface is not registered on the leaf
      std::auto_ptr<IRemoteIfc> ifc2(
         leafQ->connectInterface("bar", SampleFactory::getInstance(), 100));
      CPPUNIT_ASSERT(ifc2.get() == NULL);

      // plain interface name is valid on registerd router
      std::auto_ptr<IRemoteIfc> ifc3(
         rootQ->connectInterface("bar", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc3.get() != NULL);

      // fully qualified interface name is valid on registerd router
      std::auto_ptr<IRemoteIfc> ifc4(
         rootQ->connectInterface("/bar", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc4.get() != NULL);

      // the router domain is no valid interface
      std::auto_ptr<IRemoteIfc> ifc5(
         leafQ->connectInterface("/leaf", SampleFactory::getInstance(), 300));
      CPPUNIT_ASSERT(ifc5.get() == NULL);
   }

   void test_auth_tree_search()
   {
      Router rootRouter("root");

      Router leftRouter("left");
      InternalPort portRoot2Left;
      CPPUNIT_ASSERT(portRoot2Left.connect(rootRouter, leftRouter, "left"));

      Router rightRouter("right");
      InternalPort portRoot2Right;
      CPPUNIT_ASSERT(portRoot2Right.connect(rootRouter, rightRouter, "right"));

      std::auto_ptr<IQueue> rootQ(new Queue("root-queue", rootRouter));
      std::auto_ptr<IQueue> leftQ(new Queue("left-queue", leftRouter));
      std::auto_ptr<IQueue> rightQ(new Queue("right-queue", rightRouter));

      // registered as "/left/foo"
      std::auto_ptr<ILocalIfc> pubIfc(
         leftQ->registerInterface(SampleFactory::getInstance(), "foo"));

      // search from left router
      std::auto_ptr<IRemoteIfc> ifc1(
         leftQ->connectInterface("/left/foo", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc1.get() != NULL);

      // search from right router
      std::auto_ptr<IRemoteIfc> ifc2(
         rightQ->connectInterface("/left/foo", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc2.get() != NULL);

      // search from root router
      std::auto_ptr<IRemoteIfc> ifc3(
         rootQ->connectInterface("/left/foo", SampleFactory::getInstance(), 5000));
      CPPUNIT_ASSERT(ifc3.get() != NULL);
   }

   void test_auth_tree_timout()
   {
      Router rootRouter("root");

      Router leftRouter("left");
      InternalPort portRoot2Left;
      CPPUNIT_ASSERT(portRoot2Left.connect(rootRouter, leftRouter, "left"));

      Router rightRouter("right");
      InternalPort portRoot2Right;
      CPPUNIT_ASSERT(portRoot2Right.connect(rootRouter, rightRouter, "right"));

      std::auto_ptr<IQueue> rootQ(new Queue("root-queue", rootRouter));
      std::auto_ptr<IQueue> leftQ(new Queue("left-queue", leftRouter));
      std::auto_ptr<IQueue> rightQ(new Queue("right-queue", rightRouter));

      CPPUNIT_ASSERT(!checkIfcExist("/right/foo", leftQ.get(), 100));
      CPPUNIT_ASSERT(!checkIfcExist("/left/bar", rightQ.get(), 100));
   }

   /**
    * Register an interface from a leaf router through a stub resolver. After
    * the network split the inteface must not be registered on the root router
    * anymore. It must still be available on the leaf router, though.
    */
   void test_disconnect_stub_1()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      std::auto_ptr<InternalPort> portRoot2Leaf(new InternalPort);
      CPPUNIT_ASSERT(portRoot2Leaf->connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      // split nets
      portRoot2Leaf.reset();

      CPPUNIT_ASSERT(!checkIfcExist("foo", rootQ.get(), 500));
      CPPUNIT_ASSERT(checkIfcExist("foo", leafQ.get()));
   }

   /**
    * Register an interface from a leaf router through a stub resolver across
    * two hops. After the network split the inteface must not be registered on
    * the root router anymore. It must still be available on the leaf router,
    * though.
    *
    * root <--- mid <-X- leaf
    */
   void test_disconnect_stub_2()
   {
      Router rootRouter("root");
      Router midRouter("mid");
      InternalPort portRoot2Mid;
      CPPUNIT_ASSERT(portRoot2Mid.connect(rootRouter, midRouter));
      Router leafRouter("leaf");
      std::auto_ptr<InternalPort> portMid2Leaf(new InternalPort);
      CPPUNIT_ASSERT(portMid2Leaf->connect(midRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> midQ(new Queue("asdf", midRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      CPPUNIT_ASSERT(checkIfcExist("foo", rootQ.get()));
      CPPUNIT_ASSERT(checkIfcExist("foo", midQ.get()));
      CPPUNIT_ASSERT(checkIfcExist("foo", leafQ.get()));

      // split nets
      portMid2Leaf.reset();

      CPPUNIT_ASSERT(!checkIfcExist("foo", rootQ.get(), 500));
      CPPUNIT_ASSERT(!checkIfcExist("foo", midQ.get(), 500));
      CPPUNIT_ASSERT(checkIfcExist("foo", leafQ.get()));
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(NameServerTestSuite);
