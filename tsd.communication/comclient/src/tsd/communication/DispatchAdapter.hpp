///////////////////////////////////////////////////////
//!\file DispatchAdapter.hpp
//!\brief Adapter for mapping ComMgr events to the dispatch framework.
//!
//!\author rene.reusner@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_COMCLIENT_DISPATCHADAPTER_HPP
#define TSD_COMMUNICATION_COMCLIENT_DISPATCHADAPTER_HPP

#include <cstdio>
#include <cstring>
#include <assert.h>

#include <memory>
#include <list>

#include <tsd/common/types/typedef.hpp>
#include <tsd/common/system/Mutex.hpp>

#include <tsd/communication/IComReceive.hpp>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/common/ipc/commlib.h>

#include <tsd/common/dispatch/baseasynctransmission.hpp>
#include <tsd/common/dispatch/transmissioncontroller.hpp>
#include <tsd/common/dispatch/itransmissioncallback.hpp>


namespace tsd { namespace communication {

/** A Proxy for ComMgr events sending them to a WaitForTransmission event loop.
    Serializing is done by the comclient.
    
    using:
    
    class myclass : public IComReceive, public IEventSerializer {
       virtual void receiveEvent(event) 
       { 
          // handle event
       }
    }

    tsd::communication::ICommunicationClient *pCC = 
    tsd::communication::ICommunicationClient::getInstance();

    myclass Receiver;
    DispatchAdapter proxy(&Receiver);

    tsd::communication::TsdEventSerializer::getInstance()->addSerializer(&Receiver, event_vector);
    pCC->addObserver(&proxy, event_vector);
    
    proxy.receive()

    while(1) {
      waitForTransmission();
    }

    The DispatchAdapter proxy receives the event, uses a semaphore and a
    queue for delivering the event to Receiver. Receivers receiveEvent is
    called from waitFromTransmission if a Event arives.
*/
class DispatchAdapter
   : public tsd::communication::IComReceive
   , public tsd::common::dispatch::ITransmissionCallback
{
public:
   DispatchAdapter(IComReceive *receiver);
   DispatchAdapter();
   
   void init(IComReceive *receiver);
   
   virtual ~DispatchAdapter();
   
   virtual void receive();
   virtual void cancel();

   virtual void receiveEvent(std::auto_ptr<event::TsdEvent> event);
   virtual bool transmissionSignaled(tsd::common::ipc::Transfer& result);

private:

   void setup();
   
   tsd::common::system::Mutex                    m_QueueMutex;
   std::list<event::TsdEvent*>                   m_Queue;
   IComReceive*                                  m_Recv;
   tsd::common::ipc::Object*                     m_Sem;
   
   tsd::common::dispatch::BaseAsyncTransmission  m_BAT;
   tsd::common::ipc::Transfer                    m_Transfer;
};

} } 


#endif // TSD_COMMUNICATION_COMCLIENT_DISPATCHADAPTER_HPP
