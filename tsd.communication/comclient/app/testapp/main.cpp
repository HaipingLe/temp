/*
 *    Testapplikation zum Erzeugen von IPC - Daten bzw. Communication Manager Events
 *
 *    Clientseite erzeugt Traffic und wird mit Parameter -c gestartet
 *    Serverseite empfängt Traffic
 *    Hilfe mit Parameter -?
 *
 */
#include <cstring>
#include <tsd/common/ctassert.hpp>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tsd/common/ipc/impl/server.h>
#include <tsd/common/ipc/impl/nameserver.h>
#include <tsd/common/ipc/os.h>
#include <tsd/common/ipc/impl/arbiter.h>
#include <tsd/common/utils/endian.hpp>
#include <tsd/common/system/Clock.hpp>

#include <tsd/communication/TsdEventSerializer.hpp>
#include <tsd/communication/ICommunicationClient.hpp>
#include <tsd/communication/CommunicationClient.hpp>
#include <tsd/communication/event/TsdTemplateEvent5.hpp>
#include <tsd/communication/event/TsdTemplateEvent6.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <semaphore.h>
#include <sched.h>

timespec myTimesNS(void){
   struct timespec tim;
   clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &tim);
   //clock_gettime( CLOCK_THREAD_CPUTIME_ID, &tim);
   return tim;
}
timespec myTimesNS_MONO(void){
   struct timespec tim;
   clock_gettime( CLOCK_REALTIME, &tim);
   return tim;
}
timespec difftime(timespec start, timespec end)
{
   timespec temp;
   if ((end.tv_nsec-start.tv_nsec)<0) {
      temp.tv_sec = end.tv_sec-start.tv_sec-1;
      temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
   } else {
      temp.tv_sec = end.tv_sec-start.tv_sec;
      temp.tv_nsec = end.tv_nsec-start.tv_nsec;
   }
   return temp;
}

inline uint32_t myTimeMS(void){
   return tsd::common::system::Clock::getTickCounter();
}

#include "main.hpp"

using namespace tsd::common::ipc;
typedef ::tsd::communication::event::TsdTemplateEvent6<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, std::string> tEvent_long;

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#endif /* !defined EXIT_SUCCESS */

#ifndef EXIT_FAILURE
#define EXIT_FAILURE	1
#endif /* !defined EXIT_FAILURE */

uint32_t ID_TESTCOMMGR = 0x0AFFE000;

enum{
   optResponse = 1,
   optLoad     = 2,
   optTimeout  = 4,
   optStatCount  = 8,
};

#define MYRESPONSE_ID 64


bool bCommManager = false; //use Communicationmanager
bool bClient = false;	   //Client,Server
bool bNoAck = true;	      //no acknoledge
bool bShMem = false;	      // shmem or net
bool bVerbose = false;     //
bool bWaitResp = false;    //
uint32_t g_PktSize = 0;		//size of packets
uint32_t g_PktTimeout = 0;		//timeout between packets
uint32_t g_PktNumbers = 0;		//number of packets 0-infinite
uint32_t g_BurstNumbers = 0;  //number of packets per burst
uint32_t g_BurstTimeout = 0;
uint32_t g_PktThreads = 1;		//number of Threads
uint32_t g_ServerOpt = 0;     //ServerOptions
uint32_t g_SvrTimeout = 0;    //Server Timout or load
uint32_t g_statCount = 0;  //statistic every n events
volatile uint32_t g_PktRespCount = 0;   //
uint32_t g_DiffTxRxPkt = 0;   //diff between tx and rx packets
uint32_t g_MyEventID   = 0;   //diff between tx and rx packets

char pSName[64];	//servername number



TstClient::TstClient(void) : ::tsd::communication::IComReceive(), ::tsd::communication::IEventSerializer()
{
   m_eventid = ID_TESTCOMMGR;
   std::vector<uint32_t> events;

   if ( !bClient )
   {
      events.push_back(m_eventid | 0);
      events.push_back(m_eventid | 1);
      events.push_back(m_eventid | 2);
   }
   else
   {
      events.push_back(m_eventid | MYRESPONSE_ID );
   }

   tsd::communication::ICommunicationClient::getInstance()->addObserver(this, events);
   tsd::communication::TsdEventSerializer::getInstance()->addSerializer(this, events);
   semWait1 = tsd::common::ipc::createCountingSemaphore(0);
   semWait2 = tsd::common::ipc::createCountingSemaphore(0);
   semWait3 = tsd::common::ipc::createCountingSemaphore(0);
   //sem_init( &semQNX1, 0, 0);
   //sem_init( &semQNX2, 0, 0);
   str.clear();
   lastRxTime = maxDiffRxTime = maxDiffRxTimeEvt = 0;
   maxLatenzTime = maxLatenzTimeEvt = 0;
   last1000Time = startTimeTest = myTimeMS();
   statisticCount = 1000;
   rxcount = 0;
   rxcountA = 0;
   startTimeTest_ThreadTime = myTimesNS();
}


TstClient::~TstClient(void)
{
   tsd::communication::ICommunicationClient::getInstance()->deleteObserver(this);
   tsd::communication::TsdEventSerializer::getInstance()->removeSerializer(this);
   //sem_destroy( &semQNX1 );
   //sem_destroy( &semQNX2 );
   str.clear();
}

void
TstClient::receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event)
{
   tEvent_long* pEventRx = dynamic_cast<tEvent_long *>(event.get());
   uint32_t rxtime = myTimeMS();
   uint32_t diffrxtime = rxtime - lastRxTime;
   uint32_t count = pEventRx->getData2();
   uint32_t latenztime = rxtime-pEventRx->getData3();
   lastRxTime = rxtime;

   if ( (!bClient) && (pEventRx->getData1() != MYRESPONSE_ID) )
   {
      ++rxcount;
      ++rxcountA;
      if ( pEventRx->getData1() == 0 ) //first pkt testcase?
      {
         if (pEventRx->getData4() & optStatCount)
            statisticCount = pEventRx->getData5();
         if ( statisticCount == 0 )
            statisticCount = 1;
         printf("------------new test--------------------------------------\n");
         printf("Working load:           %d ms\n", (pEventRx->getData4()&optLoad) ?  pEventRx->getData5():0);
         printf("Working timeout:        %d ms\n", (pEventRx->getData4()&optTimeout) ?  pEventRx->getData5():0);
         printf("Wait between rx:        %d ms\n", (bWaitResp) ? g_PktTimeout:0);
         printf("Evt size       :        %d\n", pEventRx->getData6().size());
         printf("Statistic every:        %d events\n", statisticCount);
         startTimeTest = last1000Time = rxtime;
         rxcount = 1;
         maxDiffRxTime = 0;
         startTimeTest_ThreadTime = myTimesNS();
      }
      else
      {
         if (maxDiffRxTime < diffrxtime )
         {
            maxDiffRxTime = diffrxtime ;
            maxDiffRxTimeEvt = count;
         }
         if (maxLatenzTime < latenztime )
         {
            maxLatenzTime = latenztime ;
            maxLatenzTimeEvt = count;
         }
      }
      //check error packet
      if ( rxcount != count )
      {
         printf("Error rx: expected: %d received: %d\n", rxcount, count);
         rxcount = count;
      }
      if ( rxcount%statisticCount == 0 )
      {
         printf("RxEvt: Count=%d Time=%d MaxRxDiff=%d Evt=%d MaxLatenz=%d Evt=%d\n",
               rxcount, rxtime-last1000Time, maxDiffRxTime, maxDiffRxTimeEvt, maxLatenzTime, maxLatenzTimeEvt);
         last1000Time = rxtime;
         maxDiffRxTime = 0;
         maxDiffRxTimeEvt = 0;
         maxLatenzTime = maxLatenzTimeEvt = 0;
      }
      uint32_t opt = pEventRx->getData4();
      if ( opt & optLoad )
      {
         timespec start, end,timeout;
         timeout.tv_sec = 0;
         timeout.tv_nsec = (long)pEventRx->getData5()*1000000L;
         start = end = myTimesNS();
         while( difftime(start,end).tv_nsec < timeout.tv_nsec ) {
            for(uint32_t i = 0; i<1000; i++); //meaningless work
            end = myTimesNS();
         }                            //workingtime until next event
      }
      if ( opt & optTimeout )
      {
         uint32_t timeout = pEventRx->getData5();
         if ( timeout > 0 )
            SleepMS(timeout);        //waittime until get next event
      }
      if ( opt & optResponse )
      {
         send( MYRESPONSE_ID,  pEventRx->getData2(),  pEventRx->getData3() ,0 , pEventRx->getData1(), 0);
      }

      if ( bVerbose )
      {
         printf("RxEvt: Count=%d TRxCPU %d,TTxCPU=%d TDTxRx=%d TDRxRx=%d size%d\n",
               count, rxtime, pEventRx->getData3(), rxtime-pEventRx->getData3(), diffrxtime, pEventRx->getData6().size());
      }

      if ( pEventRx->getData1() == 2 ) //last pkt testcase?
      {
         printf("RxEvtEnd: Count=%d Time=%d MaxRxDiffTime=%d ms Evt=%d\n",
               rxcountA, rxtime-startTimeTest, maxDiffRxTime, maxDiffRxTimeEvt);
         timespec end =myTimesNS();
         printf("Thread_Time: %d.%ld sek\n",difftime(startTimeTest_ThreadTime,end).tv_sec,difftime(startTimeTest_ThreadTime,end).tv_nsec);
         printf("----------------------------------------------------------\n");
         rxcountA = 0;
         //tsd::common::ipc::releaseSemaphore(*semWait3);
      }
      if ( bWaitResp ) //receiving side wait
      {
         SleepMS(g_PktTimeout);
      }

   }
   else if ( (bClient) && (pEventRx->getData1() == MYRESPONSE_ID) )
   {
      uint32_t count = pEventRx->getData2();
      uint32_t startt = pEventRx->getData3();
      uint32_t time = rxtime;
      --g_DiffTxRxPkt;

      if ( ++g_PktRespCount != count )
      {
         printf("ResponseEvt: error lost Event  Expected %d count %d!!!!!!!\n", g_PktRespCount, count);
         g_PktRespCount = count;
      }
      if ( bWaitResp && (g_ServerOpt & optResponse) )
      {
            //sem_post( &semQNX );
            tsd::common::ipc::releaseSemaphore(*semWait1);
      }
      if ( bVerbose )
      {
         printf("ResponseEvt: Count=%d Timediff=%d ms ", count , time-startt);
         if ( g_DiffTxRxPkt )
            printf( "Pending Events=%d\n", g_DiffTxRxPkt);
         else
            printf("\n");
      }
      if ( pEventRx->getData1() == 2 ) //last pkt testcase?
      {
         printf("last packet!!!\n");
         tsd::common::ipc::releaseSemaphore(*semWait2);
      }
   }
   else
   {
      //printf("Rx own event! %08X\n",eid);
   }
}

void
TstClient::send(uint32_t id, uint32_t count, uint32_t timestamp, uint32_t opt, uint32_t timeout, uint32_t bufsize)
{
   bool rc;
   std::auto_ptr<tsd::communication::event::TsdEvent> evt;
   if ( bufsize != str.size() )
   {
      printf("Sendbuffer now %d was %d long, max is %d!\n", bufsize, str.size(),str.max_size() );
      str.resize(bufsize,'1');
   }
   evt.reset(new tsd::communication::event::TsdTemplateEvent6<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, std::string>(m_eventid | id, id, count, timestamp, opt, timeout, str));

   //tEvent* pEventTx = dynamic_cast<tEvent *>(evt.get());
   //printf("-->");
   //printf("TxEvt %d: EID=%08X ID=%d Count=%d Time=%d Opt=%d TOut=%d\n", myTimeMS(), pEventTx->getEventId(), pEventTx->getData1(),
   //      pEventTx->getData2(), pEventTx->getData3(), pEventTx->getData4(), pEventTx->getData5());
   rc = tsd::communication::ICommunicationClient::getInstance()->send(evt);
}

std::auto_ptr<tsd::communication::event::TsdEvent>
TstClient::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   std::auto_ptr< ::tsd::communication::event::TsdEvent> ret(NULL);
   uint32_t eventid;
   buf >> eventid;
   tEvent_long * pEvent = new tEvent_long(eventid);
   buf >> (*pEvent);
   ret.reset(pEvent);
   return ret;
}

//client over ComMgr
void cclient(void)
{
   TstClient cc ;

   uint32_t packets = g_PktNumbers;
   int id;
   uint32_t count = 0;
   uint32_t startTime = myTimeMS();
   int32_t burst = g_BurstNumbers;
   uint32_t txTime;

   printf("Test ComMgr %s \n",bClient ? "transmit side":"response side");
   printf("----------------------------------------------------------\n");
   if ( bClient ){
      printf("Timeout between events: %d ms\n", g_PktTimeout);
      printf("Number of events:       %d \n", g_PktNumbers);
      printf("Response:               %s \n",  (g_ServerOpt&optResponse) ? "on":"off");
      printf("Working load:           %d ms\n", (g_ServerOpt&optLoad) ? g_SvrTimeout:0);
      printf("Working timeout:        %d ms\n", (g_ServerOpt&optTimeout) ? g_SvrTimeout:0);
      printf("----------------------------------------------------------\n");
      cc.startTimeTest_ThreadTime = myTimesNS();
   }
   while ( bClient ) {
      txTime = myTimeMS();
      if (packets == g_PktNumbers && packets != 0) //first packet
      {
         id = 0;
         cc.last1000Time = txTime;
      }
      else if (packets == 1)  //last packet
      {
         printf("last packet!!!\n");
         id = 2;
      }
      else
      {
         id = 1;
      }
      cc.send( id, ++count, txTime, g_ServerOpt, g_SvrTimeout, g_PktSize);
      ++g_DiffTxRxPkt;
      if ( count%1000 == 0 )
      {
         printf("Tx count=%d time=%d ms\n", count, txTime-cc.last1000Time);
         cc.last1000Time = txTime;
      }
      ///
      if (  g_BurstTimeout )
      {
         SleepMS(g_BurstTimeout);
      }

      if ( g_PktTimeout )
      {
         if ( --burst <= 0 )
         {
            if ( bVerbose && g_BurstNumbers )
            {
              printf("%d burst %d ms timeout between events\n", g_BurstNumbers, g_BurstTimeout);
            }
            SleepMS(g_PktTimeout);
#if 0
            timespec sleep1 = myTimesNS_MONO();
            SleepMS(g_PktTimeout);
            timespec sleep2 = myTimesNS_MONO();
            timespec sleep3 = difftime(sleep1,sleep2);
            if ( sleep3.tv_nsec  != (int32_t)(g_PktTimeout*1000000) )
            {
               printf("TSleepError=%d.%ld %d.%ld %d.%ld \n",sleep3.tv_sec,sleep3.tv_nsec,sleep1.tv_sec,sleep1.tv_nsec,sleep2.tv_sec,sleep2.tv_nsec);
            }
#endif
            burst = g_BurstNumbers;
         }
      }

      if ( bWaitResp && (g_ServerOpt & optResponse) )
      {
            //sem_wait( &cc.semQNX );
            tsd::common::ipc::waitForSemaphore(*cc.semWait1);
      }

      if (packets) {
         if (--packets == 0)
            break;
      }
   }

   if ( bClient )
   {
      if ( (g_ServerOpt & optResponse) )
      {
         tsd::common::ipc::waitForSemaphore(*cc.semWait2);
      }
      printf("Total time:  %d ms events with resp  %d\n", myTimeMS() - startTime, g_PktRespCount);
      printf("----------------------------------------------------------\n");
      getchar();
      timespec end =myTimesNS();
      printf("Thread_Time: %d.%ld sek\n",difftime(cc.startTimeTest_ThreadTime,end).tv_sec,difftime(cc.startTimeTest_ThreadTime,end).tv_nsec);
      printf("----------------------------------------------------------\n");
   }
   else
   {
      getchar();
      //tsd::common::ipc::waitForSemaphore(*cc.semWait3);
   }
}
//Transmiter using IPC
class MyThread : public Thread
{
public:
   MyThread() :
         Thread("ClientThread")
   {
   }
   void
   run()
   {
      uint32_t packets = g_PktNumbers;
      uint32_t count = 0;
      uint32_t newbufsize = (g_PktSize < (6*sizeof(uint32_t))) ? 6 : g_PktSize/4;     //id,count,time,opt,optTime
      uint32_t sendtime, starttime, maxWaitTimeAck, waitTime;

      if (bShMem)
         createConnectServer(pSName, 4096);
      else
         createConnectServer(pSName, 12345);

      Object* snd_con = getNamedObject("tsd.mibstd2.example.snd");
      assert(snd_con);

      Transfer tx;
      uint32_t * snd_buffer = new uint32_t[newbufsize];
      printf("Test IPC %s\n",bClient ? "transmit side":"response side");
      printf("----------------------------------------------------------\n");
      if ( bClient ){
         printf("Timeout between packets: %d ms\n", g_PktTimeout);
         printf("Number of packets:       %d \n", g_PktNumbers);
         printf("Packetsize:              %d \n", g_PktSize);
         printf("Ack %s\n", bNoAck ? "off":"on");
         printf("----------------------------------------------------------\n");
      }
      starttime = myTimeMS();
      maxWaitTimeAck = 0;
      while (1) {
         if (packets == g_PktNumbers ) //first packet
            snd_buffer[0] = 0;
         else if (packets == 1)  //last packet
            snd_buffer[0] = 2;
         else
            snd_buffer[0] = 1;

         tx.init(snd_buffer, newbufsize*4, 0);

         if ( bNoAck )
            tx.m_Options = tx.fNoAck;

         sendtime = myTimeMS();
         snd_buffer[1] = ++count;
         snd_buffer[2] = sendtime;
         snd_buffer[3] = g_ServerOpt;
         snd_buffer[4] = g_SvrTimeout;
         snd_buffer[5] = myself();

         snd_con->send(tx);
         waitTime = myTimeMS()-sendtime;
         if ( waitTime > maxWaitTimeAck )
            maxWaitTimeAck = waitTime;

         if ( bVerbose )
         {
            printf("IPC-Tx%d: count=%d Timediff send<>ack %d\n", myself(), count, waitTime);
         }
         if (g_PktTimeout) {
            SleepMS(g_PktTimeout);
         }
         if (--packets == 0 && g_PktNumbers > 0){
               break;
         }
      }
      printf("IPC-Tx%d: pktsend=%d ack=%s maxWaitTime=%d overall-time=%d\n",
            myself(), count,   bNoAck ? "off":"on", maxWaitTimeAck, myTimeMS()-starttime);
      delete snd_buffer;
   }
};

//Receiver using IPC
void
server(void)
{
   uint32_t i[g_PktThreads+1] ;
   uint32_t startTime[g_PktThreads+1];
   uint32_t maxDiffTime[g_PktThreads+1];
   uint32_t last1000Time[g_PktThreads+1];
   uint32_t diffTime, actTime ;
   uint32_t newbufsize = (g_PktSize < (6*sizeof(uint32_t))) ? 6 : g_PktSize/4;     //id,count,time,opt,optTime
   ObjectPair con = createConnection();
   if ( ! registerNamedObject("tsd.mibstd2.example.snd", con.m_Sender) )
   {
      printf("registerNamedObject failed!\n");
      return;
   }

   if (bShMem)
      createListenServer(pSName, 4096);
   else
      createListenServer(pSName, 12345);

   Transfer rx;
   printf("Test IPC %s \n",bClient ? "transmit side":"response side");
   printf("----------------------------------------------------------\n");
   uint32_t * buff = new uint32_t[newbufsize];
   while (1) {
      rx.init(buff, newbufsize*4, 0);
      con.m_Receiver->receive(rx);
      uint32_t id = buff[0];
      uint32_t txcount = buff[1];
      uint32_t txTime = buff[2];
      // uint32_t opt = buff[3];
      // uint32_t svrtimout = buff[4];
      uint32_t taskid = buff[5];
      actTime = myTimeMS();
      if ( taskid > g_PktThreads )
         taskid = 0;
      if (id == 0) { //first packet
         i[taskid] = 1;
         maxDiffTime[taskid] = 0;
         startTime[taskid] = actTime;
         last1000Time[taskid] = actTime;
      } else {
         ++i[taskid];
         diffTime = actTime - txTime;
         if (diffTime > maxDiffTime[taskid]) {
            maxDiffTime[taskid] = diffTime;
         }
      }
      if ( taskid > 0 && i[taskid] != txcount ){
         printf("Error rx%d: expected: %d received: %d\n", taskid, i[taskid], txcount);
         i[taskid] = txcount;
      }
      if ( (i[taskid] % 1000) == 0 ) { //every n packets
               printf("rcv%d: size=%u pktnum=%d time=%d ms maxDiffTime=%d ms \n",taskid, rx.m_Transferred, i[taskid], actTime - last1000Time[taskid],
                     maxDiffTime[taskid]);
               maxDiffTime[taskid] = 0;
               last1000Time[taskid] = actTime;
      }
      if (id == 2 ) { //last packet
               printf("rcv%d: last pkt: size=%u pktnum=%d time=%d ms\n",
                     taskid, rx.m_Transferred, i[taskid], actTime - startTime[taskid]);
            }
   }
   delete buff;
}

std::auto_ptr<tsd::communication::event::TsdEvent>
Source(void)
{
   std::auto_ptr< ::tsd::communication::event::TsdEvent> ret(NULL);
   std::string str;
   str.resize(20,'1');
   ret.reset(new tsd::communication::event::TsdTemplateEvent6<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, std::string>
                  (0xaffe1000, 2, 3, 4, 5, 6, str));
   return ret;
}

uint32_t Sink(std::auto_ptr<tsd::communication::event::TsdEvent> event)
{
   tEvent_long* pEventRx = dynamic_cast<tEvent_long *>(event.get());
   return pEventRx->getData1();
}

int
main(int argc, char *argv[])
{
   int rc = EXIT_SUCCESS;
   char * pEnd;
   memset(pSName, 0, sizeof(pSName));
   bClient = false;
   bNoAck = true;

#if 0

   if ( argc == 2 )
   {
      switch (*argv[1])
      {
         case '1':
         {
            printf("CPU_Load!!!\n");
            int i;
            while( 1)
            {
               i++;
            }

         }
            break;
         case '2':
         {
            printf("SleepMS(10) and printf !!!\n");
            while(1)
            {
               printf("%d \n", myTimeMS() );
               SchedYield( );
            }

         }
            break;
      }

   }
#endif

   /* parse args */
   while (--argc) {
      if (**++argv != '-') {
         printf("   Arg??. = %s\n", *argv);
      } else {
         pEnd = *argv;
         ++pEnd;
         if ( *pEnd == ' ' )
            ++pEnd;
         if (*pEnd == 'c' || *pEnd == 'C') // client
            { bClient = true; }
         else if (*pEnd == 'm' || *pEnd == 'M') // shared mem
         { bShMem = true; }
         else if (*pEnd == 'v' || *pEnd == 'V') // verbose
         { bVerbose = true; }
         else if (*pEnd == 'w' || *pEnd == 'W') // wait for response
         { bWaitResp = true; }
         else if (*pEnd == 'a' || *pEnd == 'A') // with ack
         {bNoAck = false;}
         else if (*pEnd == 'e' || *pEnd == 'E') // use communication manager
         {
            if ( *pEnd == ' ' )
               ++pEnd;
            bCommManager = true;
               g_MyEventID = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
               ID_TESTCOMMGR |= (g_MyEventID<<28);
            }
            else if (*pEnd == 'o' || *pEnd == 'O') // server options
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_ServerOpt = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
            } else if (*pEnd == 'r' || *pEnd == 'R') // server timeout/workload
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_SvrTimeout = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
            } else if (*pEnd == 's' || *pEnd == 'S') // packetsize
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_PktSize = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
               if (g_PktSize == 0)
                  g_PktSize = 1;
            } else if (*pEnd == 't' || *pEnd == 'T') // timeout
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_PktTimeout = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
            } else if (*pEnd == 'n' || *pEnd == 'N') // number of packet
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_PktNumbers = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
            } else if (*pEnd == 'b' ) // number of packet in one burst
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
              g_BurstNumbers = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
            } else if (*pEnd == 'B' ) // burst timeout
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_BurstTimeout = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
            } else if (*pEnd == 'p' || *pEnd == 'P') // number of client tasks
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_PktThreads = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
               if (g_PktThreads == 0)
                  g_PktThreads = 1;
            } else if (*pEnd == 'x' || *pEnd == 'X') // servername
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               memset(pSName, 0, sizeof(pSName));
               strncpy(pSName, ++pEnd, sizeof(pSName));
               pEnd += strlen(pEnd);
            } else if (*pEnd == 'z' || *pEnd == 'Z') // statistic count
            {
               if ( *pEnd == ' ' )
                  ++pEnd;
               g_statCount = strtoul(++pEnd, &pEnd, 10);
               --pEnd;
               if (g_PktThreads == 0)
                  g_PktThreads = 1;
            } else if (*pEnd == '?' || *pEnd == 'h') // help
            {
               printf("app1 -m -r -c -a -w -v -bn -Bn -en -on-sn -tn -nn -pn -xn -zn\n");
               printf("   m - shared memory (default net)\n");
               printf("   b - burst n events (default 1) ComMgr\n");
               printf("   B - burst events with timeout between events (default 0) ComMgr\n");
               printf("   c - clientapplication(tx-side) (default server)\n");
               printf("   a - with ack (default without) IPC\n");
               printf("   e - use ComMgr (event group n) ComMgr\n");
               printf("   o - serveroptions (1 -response, 2- workload, 4- timeout, default 0) ComMgr\n");
               printf("   r - server timeout, workload (default 0) ComMgr\n");
               printf("   s - packetsize n bytes (default 512) IPC\n");
               printf("   t - timeout n ms between tx (default 0)\n");
               printf("   n - number of packets (default infinite)\n");
               printf("   p - number of client tasks (default 1) IPC\n");
               printf("   v - verbose\n");
               printf("   w - wait for response, to rx next event\n");
               printf("   z - statistic every n counts\n");
               printf("   x - servername (default shmext:TEST if -m) \n");
               printf("                  (        127.0.0.1   if -c)\n");
               printf("                  (        0.0.0.0     all other)\n");
               printf("Sample:\n");
               printf("Rx IPC: app1 -m -v\n");
               printf("Tx IPC: app1 -m -c -t10 -n 10 -v\n");
             printf("Rx ComMgr: app1 -m -e -xDEVELJ5A -v\n");
             printf("Tx ComMgr: app1 -m -e -xDEVELJ5B -c -t10 -n 10 -v\n");
             return EXIT_SUCCESS;
         }
           else
         {
            printf("??? ");
         }
         printf("   Flag = %s\n", *argv);
      }
   }
#if 0
   {
      struct _clockperiod old_clock;
      ClockPeriod( CLOCK_REALTIME, NULL,  &old_clock, 0);
      if ( old_clock.nsec >= 1000000 )
      {
         old_clock.nsec /= 4;
         ClockPeriod( CLOCK_REALTIME, &old_clock,  NULL, 0);
      }
      ClockPeriod( CLOCK_REALTIME, NULL,  &old_clock, 0);
      printf( "Clock Period is  fract=%ld,  %ld nano seconds.\n",
                old_clock.fract, old_clock.nsec );
   }
#endif
   {
      struct timespec res;
      if ( clock_getres( CLOCK_REALTIME, &res) == -1 ) {
            perror( "clock get resolution" );
            return EXIT_FAILURE;
      }
      printf( "Resolution is %ld micro seconds.\n",
                res.tv_nsec / 1000 );
   }
   //set servername to default
   if (pSName[0] == 0) {
      if (bShMem)
         strncpy(pSName, "shmext:TEST", sizeof(pSName));
      else if (bCommManager) {
         strncpy(pSName, "LOCAL", sizeof(pSName));
      } else {
         if (bClient)
            strncpy(pSName, "127.0.0.1", sizeof(pSName));
         else
            strncpy(pSName, "0.0.0.0", sizeof(pSName));
      }
   }
   printf("pSName: %s \n", pSName);
   printf("Version: %s %s\n", __DATE__, __TIME__ );

   if (bClient && !bCommManager) {    //IPC Client sending side
      while (g_PktThreads-- > 0) {
         (new MyThread)->start(10, 8 * 4096);
      }
      getchar();
   } else if (bCommManager) {
      if (bShMem)
         tsd::communication::CommunicationClient::getInstance()->init(pSName, NULL);
      else
         tsd::communication::CommunicationClient::getInstance()->init(pSName, "127.0.0.1");
      cclient();  //ComMgr test
   } else {
      server();    //IPC receiving side
   }
   return rc;
}

