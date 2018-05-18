#include <memory>
#include <stdio.h>
#include <tsd/communication/ICommunicationClient.hpp>
#include <tsd/communication/CommunicationClient.hpp>
#include <tsd/communication/TsdEventSerializer.hpp>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/event/TsdTemplateEvent2.hpp>


#include <tsd/common/system/Thread.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/communication/IEventSerializer.hpp>
#include <tsd/communication/event/TsdEvent.hpp>


class TestSerializer :  public tsd::communication::IEventSerializer
{
public:
   TestSerializer()
      {
         std::vector<uint32_t> events;
         events.push_back(1);
         tsd::communication::TsdEventSerializer::getInstance()->addSerializer(this, events);
      }

   virtual std::auto_ptr<tsd::communication::event::TsdEvent> deserialize(tsd::common::ipc::RpcBuffer& buf)
      {
         uint32_t event;
         buf >> event;
         std::auto_ptr<tsd::communication::event::TsdEvent> ret;
         ret.reset(new tsd::communication::event::TsdTemplateEvent2<int, int>(event));
         buf >> *ret;
         return ret;
      }

   virtual ~TestSerializer() {}
};


class TestClient : public tsd::communication::IComReceive,  public tsd::common::system::Thread
{
public:
   TestClient(const char*);
   virtual ~TestClient(void);

   virtual void receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event);

   void send(int id);

   std::string m_Name;
   uint32_t m_eventid;
   tsd::common::system::Mutex m_Mutex;

   volatile bool m_Stop;
   int counter;

   virtual void run();
};


TestClient::TestClient(const char* n) :tsd::common::system::Thread(n),  m_Name(n)
{
   m_Stop = false;
   start();
}


void TestClient::run()
{
   bool check;
   do {
      {
         tsd::common::system::MutexGuard g(m_Mutex);
         check = m_Stop;
      }
      std::vector<uint32_t> events;

      m_eventid = 1;
      events.push_back(m_eventid);

      tsd::communication::IComClientQueue *foo = tsd::communication::CommunicationClient::getInstance()->getQueue(m_Name, tsd::communication::ICommunicationClient::NormalQueue);
      foo->addObserver(this, events);

      delete foo;
   }  while(!check);
}


//
TestClient::~TestClient(void)
{
   {
      tsd::common::system::MutexGuard g(m_Mutex);
      m_Stop = true;
   }
   join();
}

void TestClient::receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event)
{
   tsd::communication::event::TsdTemplateEvent2<int, int> *p = dynamic_cast<tsd::communication::event::TsdTemplateEvent2<int, int> *> (event.get());

   if(p) {
      printf("Got Event Nr: %u with %d %d\n", p->getEventId(), p->getData1(), p->getData2());
   } else {
      printf("Got Event Nr:%u\n", event->getEventId());
   }
}

void TestClient::send(int id)
{
   std::auto_ptr<tsd::communication::event::TsdEvent> evt;
   evt.reset(new tsd::communication::event::TsdTemplateEvent2<int, int>(id, 1, 1));
   tsd::communication::ICommunicationClient::getInstance()->send(evt);
}




int main(int argc, char* argv[])
{
   tsd::communication::CommunicationClient *pCC = tsd::communication::CommunicationClient::getInstance();
   const char* pName = (argc > 1) ? argv[1] : "TESTCLIENT";
   pCC->init(pName, 0);

   TestSerializer ser;

   TestClient t1("Thread1");
   TestClient t2("Thread2");
   TestClient t3("Thread3");
   TestClient t4("Thread4");

   tsd::common::system::Thread::sleep(4000);
   return 0;
}

