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


#include <tsd/communication/CommunicationManager.hpp>


#define EVENT_ID1  1


class TestObserver;

static tsd::communication::CommunicationManager* cm = NULL;


tsd::common::system::Mutex observers_mutex;
std::vector<TestObserver*> observers;



class TestSerializer :  public tsd::communication::IEventSerializer
{
public:
   TestSerializer()
      {
         std::vector<uint32_t> events;
         events.push_back(EVENT_ID1);
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

   virtual ~TestSerializer() {
      tsd::communication::TsdEventSerializer::getInstance()->removeSerializer(this);
   }
};





class TestObserver : public tsd::communication::IComReceive
{
public:
   TestObserver(unsigned id);
   virtual ~TestObserver(void);

   virtual void receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event);

   void registerToQueue();
   void removeFromQueue();

   static void send(int id);


   tsd::common::system::Mutex m_Mutex;

   uint32_t m_eventid;
   unsigned m_Id;
   bool m_Active;
};


TestObserver::TestObserver(unsigned id) :  m_eventid( EVENT_ID1), m_Id(id), m_Active(false)
{
}


//
TestObserver::~TestObserver(void)
{
}

void TestObserver::receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event)
{
   {
      tsd::common::system::MutexGuard g(m_Mutex);

      assert(m_Active);
      tsd::communication::event::TsdTemplateEvent2<int, int> *p = dynamic_cast<tsd::communication::event::TsdTemplateEvent2<int, int> *> (event.get());

      if(p) {
         printf("Got Event Nr: %u with %d %d\n", p->getEventId(), p->getData1(), p->getData2());
      } else {
         printf("Got Event Nr:%u\n", event->getEventId());
      }
   }


   tsd::common::system::MutexGuard foo(observers_mutex);
   for(unsigned i = 0; i < observers.size(); i++) {
      observers[i]->removeFromQueue();
   }
}

void TestObserver::send(int id)
{
   std::auto_ptr<tsd::communication::event::TsdEvent> evt;
   evt.reset(new tsd::communication::event::TsdTemplateEvent2<int, int>(id, 1, 1));
   tsd::communication::ICommunicationClient::getInstance()->send(evt);
}



void TestObserver::registerToQueue()
{
   tsd::communication::CommunicationClient *pCC = tsd::communication::CommunicationClient::getInstance();
   tsd::common::system::MutexGuard g(m_Mutex);

   m_Active = true;

   std::vector<uint32_t> events;
   unsigned m_eventid = EVENT_ID1;
   events.push_back(m_eventid);

   pCC->addObserver(this, events);
}


void TestObserver::removeFromQueue()
{
   tsd::communication::CommunicationClient *pCC = tsd::communication::CommunicationClient::getInstance();
   tsd::common::system::MutexGuard g(m_Mutex);

   m_Active = false;

   pCC->deleteObserver(this);
}





int main()
{

   cm = new tsd::communication::CommunicationManager;
   cm->addBackend("tcp://");

   tsd::communication::CommunicationClient *pCC = tsd::communication::CommunicationClient::getInstance();
   pCC->init("LOCAL", 0, false);

   TestSerializer ser;

   std::vector<uint32_t> events;
   events.push_back(EVENT_ID1);


   {
      tsd::common::system::MutexGuard g(observers_mutex);
      for(unsigned i = 0; i < 10; i++) {
         TestObserver *p = new TestObserver(i);

         observers.push_back(p);
      }

      for(unsigned i = 0; i < observers.size(); i++) {
         observers[i]->registerToQueue();
      }
   }



   TestObserver::send(EVENT_ID1);
   tsd::common::system::Thread::sleep(4000);
   return 0;
}

