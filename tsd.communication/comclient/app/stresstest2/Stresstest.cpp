//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2012
//! TechniSat Digital GmbH
//!
//! \file    app/stresstest2/Stresstest.hpp
//! \author  Stefan Zill
//! \brief   definition of a little test app to stress the communication manager and the underlying layers with lots of packets (polygon-style transfer possible)
//!
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "Stresstest.hpp"
#include <tsd/common/ipc/rpcbuffer.h>

#include <tsd/common/system/Thread.hpp>

#include <tsd/communication/TsdEventSerializer.hpp>
#include <tsd/communication/ICommunicationClient.hpp>
#include <tsd/communication/event/TsdTemplateEvent2.hpp>

#include <sstream>

//const uint32_t ID_STRESSTEST = 0x12345670;

typedef ::tsd::communication::event::TsdTemplateEvent2<uint32_t, uint32_t> tEvent;

const uint32_t numPacketsPerId = 5;

Stresstest::Stresstest(uint32_t rxBaseId, uint32_t txBaseId)
: ::tsd::communication::IComReceive()
, ::tsd::communication::IEventSerializer()
, m_RxBaseId(rxBaseId)
, m_TxBaseId(txBaseId)
{
   std::vector<uint32_t> events;
   events.reserve(16);
   events.push_back(m_RxBaseId + 0x0);
   events.push_back(m_RxBaseId + 0x1);
   events.push_back(m_RxBaseId + 0x2);
   events.push_back(m_RxBaseId + 0x3);
   events.push_back(m_RxBaseId + 0x4);
   events.push_back(m_RxBaseId + 0x5);
   events.push_back(m_RxBaseId + 0x6);
   events.push_back(m_RxBaseId + 0x7);
   events.push_back(m_RxBaseId + 0x8);
   events.push_back(m_RxBaseId + 0x9);
   events.push_back(m_RxBaseId + 0xa);
   events.push_back(m_RxBaseId + 0xb);
   events.push_back(m_RxBaseId + 0xc);
   events.push_back(m_RxBaseId + 0xd);
   events.push_back(m_RxBaseId + 0xe);
   events.push_back(m_RxBaseId + 0xf);


   ::tsd::communication::TsdEventSerializer * pTsdSer = ::tsd::communication::TsdEventSerializer::getInstance();
   pTsdSer->addSerializer(this, events);

   ::tsd::communication::ICommunicationClient * pCC = ::tsd::communication::ICommunicationClient::getInstance();
   pCC->addObserver(this, events);
}

Stresstest::~Stresstest(void)
{
}

void Stresstest::receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event)
{
   uint32_t eventid = event->getEventId();
   if ((eventid >= m_RxBaseId) && (eventid <= m_RxBaseId  + 0xf))
   {
      uint32_t txEventId = eventid - m_RxBaseId + m_TxBaseId;
      
      tEvent* pEventRx = dynamic_cast<tEvent *>(event.get());
      tEvent* pEventTx = new tEvent(txEventId);
      uint32_t data1 = pEventRx->getData1();
      uint32_t data2 = pEventRx->getData2();
      pEventTx->setData1(data1);
      pEventTx->setData2(data2 + 1);
      
      
      if (m_ReceiveMap[eventid][data1] != data2)
      {
         ++(m_ErrorMap[eventid][data1]);
      }
      ++(m_ReceiveMap[eventid][data1]);
      
      std::auto_ptr< ::tsd::communication::event::TsdEvent> txEvent(pEventTx);
      ::tsd::communication::ICommunicationClient * pCC = ::tsd::communication::ICommunicationClient::getInstance();
      pCC->send(txEvent);
   }
}

std::auto_ptr< ::tsd::communication::event::TsdEvent> Stresstest::deserialize(::tsd::common::ipc::RpcBuffer& buf)
{
   std::auto_ptr< ::tsd::communication::event::TsdEvent> ret(NULL);
   uint32_t eventid;
   buf >> eventid;

   if ((eventid >= m_RxBaseId) && (eventid <= m_RxBaseId  + 0xf))
   {
      tEvent * pEvent = new tEvent(eventid);
      buf >> (*pEvent);
      ret.reset(pEvent);
   }

   return ret;
}

void Stresstest::start()
{
   ::tsd::communication::ICommunicationClient * pCC = ::tsd::communication::ICommunicationClient::getInstance();
   for (uint32_t i = m_RxBaseId; i <= (m_RxBaseId + 0xf); ++i)
   {
      m_ReceiveMap[i].resize(numPacketsPerId);
      m_ErrorMap[i].resize(numPacketsPerId);
      for (uint32_t j = 0; j < numPacketsPerId; ++j)
      {
         tEvent * pEvent = new tEvent(i);
         pEvent->setData1(j);
         pEvent->setData2(0);
         
         std::auto_ptr<tsd::communication::event::TsdEvent> txEvent(pEvent);
         pCC->send(txEvent);
      }
   }
}

void Stresstest::print()
{
   std::cout << "receive counts:" << &std::endl;
   for (uint32_t i = m_RxBaseId; i <= (m_RxBaseId + 0xf); ++i)
   {
      std::cout << &std::hex << "0x" << i << ": " << &std::dec;
      for (uint32_t j = 0; j < numPacketsPerId; ++j)
      {
         std::cout << m_ReceiveMap[i][j] << "/" << m_ErrorMap[i][j] << " ";
      }
      std::cout << &std::endl;
   }
   std::cout << &std::endl;
}

int main(int argc, char* argv[])
{
   // 1. Arg: NAME der Connection
   // 2. Arg: <ignoriert>
   // 3. Arg: RX-ID-Base
   // 4. Arg: TX-ID-Base
   // Es werden mit RX-ID-Base (.. RX-ID-Base+15) Events empfangen und mit ID TX-ID-Base (.. TX-ID-Base+15) wieder losgeschickt
   // Test mit Event-Schicken im Kreis möglich  1. A->B 2. B->C 3. C->A ... beliebig erweiterbar...
   
   ::tsd::communication::ICommunicationClient *pCC = ::tsd::communication::ICommunicationClient::getInstance();
   const char * pIP = 0;
   pIP = NULL; //(argc > 2) ? argv[2] : 0;
   const char* pName = (argc > 1) ? argv[1] : "NAVI";
   pCC->init(pName, pIP);
   
   uint32_t rxId = 0x1234560;
   uint32_t txId = 0x1234560;
   
   if (argc > 4)
   {
      std::stringstream ss1(argv[3]);
      ss1 >> rxId;
      
      std::stringstream ss2(argv[4]);
      ss2 >> txId;
   }

   Stresstest stress(rxId, txId);
   stress.start();
   while (true)
   {
      tsd::common::system::Thread::getCurrentThread().sleep(1000);
      stress.print();
   }
   return 0;
}

