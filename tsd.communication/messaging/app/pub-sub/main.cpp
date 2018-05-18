#include <iostream>

#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>

#include <tsd/communication/messaging/Queue.hpp>

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

/*
 * Local interface.
 */

static const uint32_t QUIT_IND = 1;

class QuitInd
   : public TsdEvent
{
public:
   QuitInd() : TsdEvent(QUIT_IND) { }
   void serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const { }
   void deserialize(tsd::common::ipc::RpcBuffer& /*buf*/) { }
   TsdEvent* clone(void) const { return new QuitInd; }
};

/*
 * Tuner status interface
 */

static const uint32_t QUALITY_IND = 1;

class QualityInd
   : public TsdEvent
{
   float m_quality;

public:
   QualityInd()
      : TsdEvent(QUALITY_IND)
      , m_quality(0)
   { }

   QualityInd(float quality)
      : TsdEvent(QUALITY_IND)
      , m_quality(quality)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_quality;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_quality;
   }

   TsdEvent* clone(void) const
   {
      return new QualityInd(m_quality);
   }

   inline float getQuality() const { return m_quality; }
   inline void setQuality(float quality) { m_quality = quality; }
};


class TunerStatusFactory
   : public IMessageFactory
{
public:
   std::auto_ptr<TsdEvent> createEvent(uint32_t msgId) const
   {
      std::auto_ptr<TsdEvent> ret;

      switch (msgId) {
         case QUIT_IND:
            ret.reset(new QualityInd);
            break;
      }

      return ret;
   }

   static IMessageFactory& getInstance()
   {
      static TunerStatusFactory factory;
      return factory;
   }
};

class Subscriber
   : public tsd::common::system::Thread
{
   IQueue *m_queue;

public:
   Subscriber()
      : tsd::common::system::Thread("listener")
      , m_queue(createQueue("listener"))
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
   }

   void run()
   {
      std::cout << "Subscriber looking for interface..." << &std::endl;

      IRemoteIfc *m_tunerIfc = m_queue->connectInterface(
            "tsd.mib3.radio.events", TunerStatusFactory::getInstance());

      std::cout << "Subscriber waiting for messages..." << &std::endl;

      bool alive = true;
      while (alive) {
         std::auto_ptr<TsdEvent> msg = m_queue->readMessage();

         // provide dispatch class for the following code
         if (msg->getReceiverAddr() == LOOPBACK_ADDRESS) {
            switch (msg->getEventId()) {
               case QUIT_IND:
                  std::cout << "Server got quit message" << &std::endl;
                  alive = false;
                  break;
               default:
                  std::cout << "Unknown message on loopback iterface: " << msg->getEventId()
                            << &std::endl;
                  break;
            }
         } else if (msg->getReceiverAddr() == m_tunerIfc->getLocalIfcAddr()) {
            switch (msg->getEventId()) {
               case QUALITY_IND:
               {
                  QualityInd *ind = dynamic_cast<QualityInd*>(msg.get());
                  std::cout << "Subscriber: got signal quality: " << ind->getQuality()
                            << &std::endl;
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

      delete m_tunerIfc;
   }
};


int main()
{
   Subscriber subscriber;
   subscriber.start();

   IQueue *q = createQueue("tsd.radio.events"); // globally unique name, fail if already claimed
   ILocalIfc *statusIfc = q->registerInterface(TunerStatusFactory::getInstance(),
         "tsd.mib3.radio.events");

   for (int i = 0; i < 3; i++) {
      tsd::common::system::Thread::sleep(300);
      // send multicast message to subscribers
      statusIfc->broadcastMessage(std::auto_ptr<TsdEvent>(new QualityInd(i * 0.2f)));
   }

   return 0;
}
