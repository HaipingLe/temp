#ifndef _MAINAPP1_H_
#define _MAINAPP1_H_
#include <memory>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/communication/IEventSerializer.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

class TstClient : public tsd::communication::IComReceive , public tsd::communication::IEventSerializer
{
public:
   TstClient(void);
   virtual ~TstClient(void);

   virtual void receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event);

   void send(uint32_t id,uint32_t count,uint32_t time,uint32_t opt, uint32_t timeout, uint32_t bufsize);

   std::string m_Name;
   uint32_t m_eventid;

   //sem_t semQNX1;
   //sem_t semQNX1;
   tsd::common::ipc::Object* semWait1;
   tsd::common::ipc::Object* semWait2;
   tsd::common::ipc::Object* semWait3;
   std::string str;
   uint32_t lastRxTime;
   uint32_t startTimeTest;
   timespec startTimeTest_ThreadTime;
   uint32_t last1000Time;
   uint32_t maxDiffRxTime;
   uint32_t maxDiffRxTimeEvt;
   uint32_t maxLatenzTime;
   uint32_t maxLatenzTimeEvt;
   uint32_t statisticCount;
   uint32_t rxcount;
   uint32_t rxcountA;
   virtual std::auto_ptr<tsd::communication::event::TsdEvent> deserialize(tsd::common::ipc::RpcBuffer&);
};


#endif // _MAINAPP1_H_

