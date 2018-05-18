//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2012
//! TechniSat Digital GmbH
//!
//! \file    app/stresstest/Stresstest.hpp
//! \author  Stefan Zill
//! \brief   declaration of a little test app to stress the communication manager and the underlying layers with lots of packets
//!
//////////////////////////////////////////////////////////////////////
#ifndef _STRESSTEST_H_
#define _STRESSTEST_H_

#include <tsd/communication/IEventSerializer.hpp>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/common/system/Mutex.hpp>

#include <map>
#include <vector>

class Stresstest : public ::tsd::communication::IEventSerializer
                 , public ::tsd::communication::IComReceive
    
{
public:
   Stresstest();
   virtual ~Stresstest();

   virtual void receiveEvent(std::auto_ptr< ::tsd::communication::event::TsdEvent> event);
   virtual std::auto_ptr< ::tsd::communication::event::TsdEvent> deserialize(::tsd::common::ipc::RpcBuffer& buf);

   void start();
   void print();
   
private:
   std::map<uint32_t, std::vector<uint32_t> > m_ReceiveMap;
   std::map<uint32_t, std::vector<uint32_t> > m_ErrorMap;
};

#endif // _STRESSTEST_H_
