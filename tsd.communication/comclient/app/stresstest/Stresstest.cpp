//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2012
//! TechniSat Digital GmbH
//!
//! \file    app/stresstest/Stresstest.cpp
//! \author  Stefan Zill
//! \brief   definition of a little test app to stress the communication manager and the underlying layers with lots of packets
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

const uint32_t ID_STRESSTEST = 0x12345670;

typedef ::tsd::communication::event::TsdTemplateEvent2<uint32_t, uint32_t> tEvent;


Stresstest::Stresstest()
: ::tsd::communication::IComReceive()
, ::tsd::communication::IEventSerializer()
{
   std::vector<uint32_t> events;
   events.reserve(16);
   events.push_back(ID_STRESSTEST | 0x0);
   events.push_back(ID_STRESSTEST | 0x1);
   events.push_back(ID_STRESSTEST | 0x2);
   events.push_back(ID_STRESSTEST | 0x3);
   events.push_back(ID_STRESSTEST | 0x4);
   events.push_back(ID_STRESSTEST | 0x5);
   events.push_back(ID_STRESSTEST | 0x6);
   events.push_back(ID_STRESSTEST | 0x7);
   events.push_back(ID_STRESSTEST | 0x8);
   events.push_back(ID_STRESSTEST | 0x9);
   events.push_back(ID_STRESSTEST | 0xa);
   events.push_back(ID_STRESSTEST | 0xb);
   events.push_back(ID_STRESSTEST | 0xc);
   events.push_back(ID_STRESSTEST | 0xd);
   events.push_back(ID_STRESSTEST | 0xe);
   events.push_back(ID_STRESSTEST | 0xf);


   ::tsd::communication::TsdEventSerializer * pTsdSer = ::tsd::communication::TsdEventSerializer::getInstance();
   pTsdSer->addSerializer(this, events);

   ::tsd::communication::ICommunicationClient * pCC = ::tsd::communication::ICommunicationClient::getInstance();

   events.clear();
   events.push_back(ID_STRESSTEST | 0x0);
   events.push_back(ID_STRESSTEST | 0x1);
   events.push_back(ID_STRESSTEST | 0x2);
   events.push_back(ID_STRESSTEST | 0x3);
   events.push_back(ID_STRESSTEST | 0x4);
   events.push_back(ID_STRESSTEST | 0x5);
   events.push_back(ID_STRESSTEST | 0x6);
   events.push_back(ID_STRESSTEST | 0x7);
   pCC->getQueue("one", tsd::communication::ICommunicationClient::RealtimeQueue)->addObserver(this, events);

   events.clear();
   events.push_back(ID_STRESSTEST | 0x8);
   events.push_back(ID_STRESSTEST | 0x9);
   events.push_back(ID_STRESSTEST | 0xa);
   events.push_back(ID_STRESSTEST | 0xb);
   events.push_back(ID_STRESSTEST | 0xc);
   events.push_back(ID_STRESSTEST | 0xd);
   events.push_back(ID_STRESSTEST | 0xe);
   events.push_back(ID_STRESSTEST | 0xf);
   pCC->getQueue("two", tsd::communication::ICommunicationClient::NormalQueue)->addObserver(this, events);
}

Stresstest::~Stresstest(void)
{
}

void Stresstest::receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event)
{
   uint32_t eventid = event->getEventId();
   switch (eventid)
   {
      case ID_STRESSTEST | 0x0:
      case ID_STRESSTEST | 0x1:
      case ID_STRESSTEST | 0x2:
      case ID_STRESSTEST | 0x3:
      case ID_STRESSTEST | 0x4:
      case ID_STRESSTEST | 0x5:
      case ID_STRESSTEST | 0x6:
      case ID_STRESSTEST | 0x7:
      case ID_STRESSTEST | 0x8:
      case ID_STRESSTEST | 0x9:
      case ID_STRESSTEST | 0xa:
      case ID_STRESSTEST | 0xb:
      case ID_STRESSTEST | 0xc:
      case ID_STRESSTEST | 0xd:
      case ID_STRESSTEST | 0xe:
      case ID_STRESSTEST | 0xf:
         {
            tEvent* pEventRx = dynamic_cast<tEvent *>(event.get());
            tEvent* pEventTx = new tEvent(eventid);
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
         break;
         
      default:
         break;
   }
}

std::auto_ptr< ::tsd::communication::event::TsdEvent> Stresstest::deserialize(::tsd::common::ipc::RpcBuffer& buf)
{
   std::auto_ptr< ::tsd::communication::event::TsdEvent> ret(NULL);
   uint32_t eventid;
   buf >> eventid;

   switch (eventid)
   {
      case ID_STRESSTEST | 0x0:
      case ID_STRESSTEST | 0x1:
      case ID_STRESSTEST | 0x2:
      case ID_STRESSTEST | 0x3:
      case ID_STRESSTEST | 0x4:
      case ID_STRESSTEST | 0x5:
      case ID_STRESSTEST | 0x6:
      case ID_STRESSTEST | 0x7:
      case ID_STRESSTEST | 0x8:
      case ID_STRESSTEST | 0x9:
      case ID_STRESSTEST | 0xa:
      case ID_STRESSTEST | 0xb:
      case ID_STRESSTEST | 0xc:
      case ID_STRESSTEST | 0xd:
      case ID_STRESSTEST | 0xe:
      case ID_STRESSTEST | 0xf:
         {
            tEvent * pEvent = new tEvent(eventid);
            buf >> (*pEvent);
            ret.reset(pEvent);
         }
         break;
         
      default:
         break;
   }

   return ret;
}

const uint32_t numPacketsPerId = 5;

void Stresstest::start()
{
   ::tsd::communication::ICommunicationClient * pCC = ::tsd::communication::ICommunicationClient::getInstance();
   for (uint32_t i = ID_STRESSTEST; i <= (ID_STRESSTEST | 0xf); ++i)
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
   for (uint32_t i = ID_STRESSTEST; i <= (ID_STRESSTEST | 0xf); ++i)
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
   ::tsd::communication::ICommunicationClient *pCC = ::tsd::communication::ICommunicationClient::getInstance();
   const char * pIP = 0;
   pIP = (argc > 2) ? argv[2] : 0;
   const char* pName = (argc > 1) ? argv[1] : "NAVI";
   pCC->init(pName, pIP);

   Stresstest stress;
   stress.start();
   while (true)
   {
      tsd::common::system::Thread::getCurrentThread().sleep(1000);
      stress.print();
   }
   return 0;
}

