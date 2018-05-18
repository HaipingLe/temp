///////////////////////////////////////////////////////
//!\file DispatchAdapter.cpp
//!\brief Adapter for mapping ComMgr events to the dispatch framework.
//!
//!\author rene.reusner@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <cstdio>

#include <memory>

#include <tsd/communication/DispatchAdapter.hpp>
#include <tsd/communication/CommunicationClient.hpp>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/communication/TsdEventSerializer.hpp>

#include <tsd/communication/event/EventMasksAdm.hpp>
#include <tsd/communication/event/TsdTemplateEvent2.hpp>



#include <tsd/common/errors/Error.hpp>

#include <tsd/common/dispatch/serializer.hpp>
#include <tsd/common/dispatch/teventlistenerregistrator.hpp>
#include <tsd/common/dispatch/osobject.hpp>


using namespace tsd::common;

namespace tsd { namespace communication {


DispatchAdapter::DispatchAdapter(IComReceive *receiver)
   : m_QueueMutex(),
     m_Queue(),
     m_Recv(receiver),
     m_Sem(NULL),
     m_BAT(),
     m_Transfer()
{
   setup();
}
      

DispatchAdapter::DispatchAdapter()
   : m_QueueMutex(),
     m_Queue(),
     m_Recv(NULL),
     m_Sem(NULL),
     m_BAT(),
     m_Transfer()
{
   setup();
}


DispatchAdapter::~DispatchAdapter()
{
   
   tsd::common::system::MutexGuard guard(m_QueueMutex);

   /* call cancel here, if the ctor is called from
      another Thread and cancel will crash, then you have a
      bug. Make sure to cancel all operation on the 
      dispatcher thread before, before deleting the object */
   DispatchAdapter::cancel();

   tsd::communication::ICommunicationClient::getInstance()->deleteObserver(this);
   

   while (!m_Queue.empty()) {
      delete m_Queue.front();
      m_Queue.pop_front();
   }

   tsd::common::ipc::unlinkObject(m_Sem);
   m_Sem = NULL;
   m_Recv = NULL;
   
}


void
DispatchAdapter::init(IComReceive *receiver)
{
   m_Recv = receiver;
}


void
DispatchAdapter::setup()
{
   m_Sem = ipc::createCountingSemaphore(0);
   m_Transfer.init(0, 0, 0);
   
   if(m_Sem == NULL)
      throw tsd::common::errors::Error("DispatchAdapter::setup CreateSemaphore failed");

   m_BAT.registerCallback(this);
}

void
DispatchAdapter::cancel()
{
   m_BAT.cancel();
}


void
DispatchAdapter::receiveEvent(std::auto_ptr<event::TsdEvent> event)
{
   tsd::common::system::MutexGuard guard(m_QueueMutex);
   
   m_Queue.push_back(event.release());
   ipc::releaseSemaphore(*m_Sem);
}


void 
DispatchAdapter::receive()
{
   m_Transfer.init(0, 0, 0);
   m_BAT.asynchronousReceive(*m_Sem, m_Transfer);
}
      
bool 
DispatchAdapter::transmissionSignaled(tsd::common::ipc::Transfer& result)
{

   assert(result.m_Status != ipc::Transfer::WantReply);
   
   if ((result.m_Status == ipc::Transfer::Finished)
       || (result.m_Status == ipc::Transfer::WantReply)) {

      event::TsdEvent *p = 0;

      {
         tsd::common::system::MutexGuard guard(m_QueueMutex);

         if (!m_Queue.empty()) {
            p = m_Queue.front();
            m_Queue.pop_front();
         }
      }      

      if(p) {
         std::auto_ptr<event::TsdEvent> event(p);
         
         if(m_Recv) {
            m_Recv->receiveEvent(event);
         }
      }
   }
   
   receive();
   return true;
}
      

} }
