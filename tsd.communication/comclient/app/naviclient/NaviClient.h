//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2012
//! TechniSat Digital GmbH
//!
//! \file    app/naviclient/Naviclient.h
//! \author  Martin Franz
//! \brief   declaration of a little test app for communication manager performance  
//!
//////////////////////////////////////////////////////////////////////
#ifndef _NAVICLIENT_H_
#define _NAVICLIENT_H_

#include <memory>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/communication/IEventSerializer.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

class NaviClient : public tsd::communication::IComReceive, public tsd::communication::IEventSerializer
{
public:
   NaviClient(const char*);
   virtual ~NaviClient(void);

   virtual void receiveEvent(std::auto_ptr<tsd::communication::event::TsdEvent> event);
      
   void send(int id);

   uint64_t microtime();
   std::string m_Name;
   uint32_t m_eventid;
   tsd::common::system::Mutex m_Mux; //!< singleton protector
   std::vector<tsd::communication::event::TsdEvent*> msg_array;
   int counter;
   
//   virtual void serialize(tsd::common::ipc::RpcBuffer&, const tsd::communication::TsdEvent&);
   virtual std::auto_ptr<tsd::communication::event::TsdEvent> deserialize(tsd::common::ipc::RpcBuffer&);
};



#endif // _NAVICLIENT_H_
