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


class MonitorTestSuite: public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(MonitorTestSuite);

   // test cases where everybody is alive
   CPPUNIT_TEST(test_hop_alive);

   // test cases where server dies and client is notified
   CPPUNIT_TEST(test_server_local_alive);
   CPPUNIT_TEST(test_server_local_dead_1);
   CPPUNIT_TEST(test_server_local_dead_2);
   CPPUNIT_TEST(test_server_remote_alive_1);
   CPPUNIT_TEST(test_server_remote_alive_2);
   CPPUNIT_TEST(test_server_remote_split_1);
   CPPUNIT_TEST(test_server_remote_split_2);
   CPPUNIT_TEST(test_server_remote_split_3);
   CPPUNIT_TEST(test_server_remote_split_4);

   // test cases where client dies and server is notified
   CPPUNIT_TEST(test_client_local_alive);
   CPPUNIT_TEST(test_client_local_dead);
   CPPUNIT_TEST(test_client_remote_alive_1);
   CPPUNIT_TEST(test_client_remote_alive_2);
   CPPUNIT_TEST(test_client_remote_split_1);

   // test demonitor
   CPPUNIT_TEST(test_demonitor_alive);
   CPPUNIT_TEST(test_demonitor_dead);

   CPPUNIT_TEST_SUITE_END();

public:
   /**
    * Negative regression test that no monitor is fired if everything is alive.
    *
    * A client is looking up a server on the same router that is a stub
    * resolver.  Two messages are passed and the monitor message must not be
    * received because the server stays alive.
    */
   void test_hop_alive()
   {
      Router rootRouter("root");

      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      portRoot2Leaf.connect(rootRouter, leafRouter);

      std::auto_ptr<IQueue> pubQ(new Queue("pub", leafRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("sub", leafRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new DummyInd));

      // ping-pong
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new StringInd("register")));

      std::auto_ptr<TsdEvent> regMsg = pubQ->readMessage(1000);
      StringInd *regMsgReal = dynamic_cast<StringInd*>(regMsg.get());
      CPPUNIT_ASSERT(regMsgReal != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("register"), regMsgReal->m_string);

      pubIfc->sendMessage(regMsgReal->getSenderAddr(), std::auto_ptr<TsdEvent>(new StringInd("ok")));

      std::auto_ptr<TsdEvent> okMsg = subQ->readMessage(1000);
      StringInd *okMsgReal = dynamic_cast<StringInd*>(okMsg.get());
      CPPUNIT_ASSERT(okMsgReal != NULL);
      CPPUNIT_ASSERT_EQUAL(std::string("ok"), okMsgReal->m_string);

      // no monitor!
      std::auto_ptr<TsdEvent> m = subQ->readMessage(300);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) == NULL);
   }

   /**
    * Test server de-registration on same router.
    */
   void test_server_local_alive()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubIfc.get(), SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      pubIfc.reset();
      std::auto_ptr<TsdEvent> m = subQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test monitor registration on dead interface on same router. The interface
    * was connected before the server vanishes.
    */
   void test_server_local_dead_1()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubIfc.get(), SampleFactory::getInstance()));

      pubIfc.reset();
      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      std::auto_ptr<TsdEvent> m = subQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test monitor registration on dead interface on same router. The interface
    * is already dead when connecting.
    */
   void test_server_local_dead_2()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      IfcAddr_t pubAddr = pubIfc->getLocalIfcAddr();

      pubIfc.reset();

      std::auto_ptr<Queue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubAddr, SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      std::auto_ptr<TsdEvent> m = subQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test de-registration monitor across networks. The server is registered
    * downstream.
    */
   void test_server_remote_alive_1()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rootQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      pubIfc.reset();
      std::auto_ptr<TsdEvent> m = rootQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test de-registration monitor across networks. The server is registered
    * upstream.
    */
   void test_server_remote_alive_2()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         rootQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         leafQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      pubIfc.reset();
      std::auto_ptr<TsdEvent> m = leafQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test monitor when connection between routers breaks.
    */
   void test_server_remote_split_1()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      std::auto_ptr<InternalPort> portRoot2Leaf(new InternalPort);
      CPPUNIT_ASSERT(portRoot2Leaf->connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rootQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      // split nets
      portRoot2Leaf.reset();

      std::auto_ptr<TsdEvent> m = rootQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test monitor when connection between routers breaks across two hops:
    *
    * root <--- mid <-X- leaf
    */
   void test_server_remote_split_2()
   {
      Router rootRouter("root");

      Router midRouter("mid");
      InternalPort portRoot2Mid;
      CPPUNIT_ASSERT(portRoot2Mid.connect(rootRouter, midRouter));

      Router leafRouter("leaf");
      std::auto_ptr<InternalPort> portMid2Leaf(new InternalPort);
      CPPUNIT_ASSERT(portMid2Leaf->connect(midRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rootQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      // split nets
      portMid2Leaf.reset();

      std::auto_ptr<TsdEvent> m = rootQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test monitor when connection between routers breaks across two hops:
    *
    *         /-X- right
    * root <-+
    *         \--- left
    */
   void test_server_remote_split_3()
   {
      Router rootRouter("root");

      Router leftRouter("left");
      InternalPort portRoot2Left;
      CPPUNIT_ASSERT(portRoot2Left.connect(rootRouter, leftRouter));

      Router rightRouter("right");
      std::auto_ptr<InternalPort> portRoot2Right(new InternalPort);
      CPPUNIT_ASSERT(portRoot2Right->connect(rootRouter, rightRouter));

      std::auto_ptr<IQueue> leftQ(new Queue("subscriver", leftRouter));
      std::auto_ptr<IQueue> rightQ(new Queue("publisher", rightRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leftQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rightQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      // split nets
      portRoot2Right.reset();

      std::auto_ptr<TsdEvent> m = rightQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**
    * Test monitor when connection between routers breaks across two hops:
    *
    *         /--- right
    * root <-+
    *         \-X- left
    */
   void test_server_remote_split_4()
   {
      Router rootRouter("root");

      Router leftRouter("left");
      std::auto_ptr<InternalPort> portRoot2Left(new InternalPort);
      CPPUNIT_ASSERT(portRoot2Left->connect(rootRouter, leftRouter));

      Router rightRouter("right");
      InternalPort portRoot2Right;
      CPPUNIT_ASSERT(portRoot2Right.connect(rootRouter, rightRouter));

      std::auto_ptr<IQueue> leftQ(new Queue("subscriver", leftRouter));
      std::auto_ptr<IQueue> rightQ(new Queue("publisher", rightRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leftQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rightQ->connectInterface("foo", SampleFactory::getInstance()));

      subIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd));

      // split nets
      portRoot2Left.reset();

      std::auto_ptr<TsdEvent> m = rightQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
   }

   /**************************************************************************/

   /**
    * Test client de-registration on same router.
    */
   void test_client_local_alive()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubIfc.get(), SampleFactory::getInstance()));

      // send message from client to server
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = pubQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // kill client and wait for monitor on server
      subIfc.reset();
      m = pubQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
      CPPUNIT_ASSERT(m->getSenderAddr() == remoteClientAddr);
      CPPUNIT_ASSERT(m->getReceiverAddr() == pubIfc->getLocalIfcAddr());
   }

   /**
    * Test monitor registration on dead client on same router. The interface
    * was connected before the server vanishes.
    */
   void test_client_local_dead()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubIfc.get(), SampleFactory::getInstance()));

      // send message from client to server and kill immediately
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));
      subIfc.reset();

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = pubQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // wait for monitor on server
      m = pubQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
      CPPUNIT_ASSERT(m->getSenderAddr() == remoteClientAddr);
      CPPUNIT_ASSERT(m->getReceiverAddr() == pubIfc->getLocalIfcAddr());
   }

   /**
    * Test client de-registration monitor across networks. The server is
    * registered downstream.
    */
   void test_client_remote_alive_1()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leafQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rootQ->connectInterface("foo", SampleFactory::getInstance()));

      // send message from client to server
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = leafQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // kill client and wait for monitor on server
      subIfc.reset();
      m = leafQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
      CPPUNIT_ASSERT(m->getSenderAddr() == remoteClientAddr);
      CPPUNIT_ASSERT(m->getReceiverAddr() == pubIfc->getLocalIfcAddr());
   }

   /**
    * Test client de-registration monitor across networks. The server is
    * registered upstream.
    */
   void test_client_remote_alive_2()
   {
      Router rootRouter("root");
      Router leafRouter("leaf");
      InternalPort portRoot2Leaf;
      CPPUNIT_ASSERT(portRoot2Leaf.connect(rootRouter, leafRouter));

      std::auto_ptr<IQueue> rootQ(new Queue("subscriver", rootRouter));
      std::auto_ptr<IQueue> leafQ(new Queue("publisher", leafRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         rootQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         leafQ->connectInterface("foo", SampleFactory::getInstance()));

      // send message from client to server and immediately kill client
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));
      subIfc.reset();

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = rootQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // wait for monitor on server
      m = rootQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
      CPPUNIT_ASSERT(m->getSenderAddr() == remoteClientAddr);
      CPPUNIT_ASSERT(m->getReceiverAddr() == pubIfc->getLocalIfcAddr());
   }

   /**
    * Test monitor when connection between routers breaks across two hops:
    *
    *         /-X- right
    * root <-+
    *         \--- left
    */
   void test_client_remote_split_1()
   {
      Router rootRouter("root");

      Router leftRouter("left");
      InternalPort portRoot2Left;
      CPPUNIT_ASSERT(portRoot2Left.connect(rootRouter, leftRouter));

      Router rightRouter("right");
      std::auto_ptr<InternalPort> portRoot2Right(new InternalPort);
      CPPUNIT_ASSERT(portRoot2Right->connect(rootRouter, rightRouter));

      std::auto_ptr<IQueue> leftQ(new Queue("subscriver", leftRouter));
      std::auto_ptr<IQueue> rightQ(new Queue("publisher", rightRouter));

      std::auto_ptr<ILocalIfc> pubIfc(
         leftQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IRemoteIfc> subIfc(
         rightQ->connectInterface("foo", SampleFactory::getInstance()));

      // send message from client to server
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = leftQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // split nets
      portRoot2Right.reset();
      //subIfc.reset();

      // wait for monitor on server
      m = leftQ->readMessage(3000);
      CPPUNIT_ASSERT(dynamic_cast<QuitInd*>(m.get()) != NULL);
      CPPUNIT_ASSERT(m->getSenderAddr() == remoteClientAddr);
      CPPUNIT_ASSERT(m->getReceiverAddr() == pubIfc->getLocalIfcAddr());
   }

   /**************************************************************************/

   /**
    * Test monitor/demonitor on alive interface. When the interface vanishes
    * the monitor must not trigger.
    */
   void test_demonitor_alive()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubIfc.get(), SampleFactory::getInstance()));

      // send message from client to server
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = pubQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      MonitorRef_t mon = pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // demonitor client
      pubIfc->demonitor(mon);

      // kill client and wait for monitor on server
      subIfc.reset();
      m = pubQ->readMessage(500);
      CPPUNIT_ASSERT(m.get() == NULL);
   }

   /**
    * Test monitor/demonitor on dead interface. The interface is already dead
    * when demonitor() is called. The monitor message must not be received.
    */
   void test_demonitor_dead()
   {
      Router rootRouter("root");

      std::auto_ptr<IQueue> pubQ(new Queue("root-pub", rootRouter));
      std::auto_ptr<ILocalIfc> pubIfc(
         pubQ->registerInterface(SampleFactory::getInstance(), "foo"));

      std::auto_ptr<IQueue> subQ(new Queue("root-sub", rootRouter));
      std::auto_ptr<IRemoteIfc> subIfc(
         subQ->connectInterface(pubIfc.get(), SampleFactory::getInstance()));

      // send message from client to server
      subIfc->sendMessage(std::auto_ptr<TsdEvent>(new FooInd));

      // receive on server and monitor client
      std::auto_ptr<TsdEvent> m = pubQ->readMessage();
      CPPUNIT_ASSERT(dynamic_cast<FooInd*>(m.get()) != NULL);
      IfcAddr_t remoteClientAddr = m->getSenderAddr();
      MonitorRef_t mon = pubIfc->monitor(std::auto_ptr<TsdEvent>(new QuitInd), remoteClientAddr);

      // kill client -> activate monitor
      subIfc.reset();

      // demonitor -> must prune message from queue
      pubIfc->demonitor(mon);

      // wait for monitor on server
      m = pubQ->readMessage(500);
      CPPUNIT_ASSERT(m.get() == NULL);
   }








   // test join dead remote interface
};

CPPUNIT_TEST_SUITE_REGISTRATION(MonitorTestSuite);
