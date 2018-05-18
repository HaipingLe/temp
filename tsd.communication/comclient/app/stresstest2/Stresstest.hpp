//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2012
//! TechniSat Digital GmbH
//!
//! \file    app/stresstest2/Stresstest.hpp
//! \author  Stefan Zill
//! \brief   declaration of a little test app to stress the communication manager and the underlying layers with lots of packets (polygon-style transfer possible)
//!
//////////////////////////////////////////////////////////////////////
#ifndef _STRESSTEST_H_2
#define _STRESSTEST_H_2

#include <tsd/communication/IEventSerializer.hpp>
#include <tsd/communication/IComReceive.hpp>
#include <tsd/common/system/Mutex.hpp>

#include <map>
#include <vector>

class Stresstest : public ::tsd::communication::IEventSerializer
                 , public ::tsd::communication::IComReceive
    
{
public:
   Stresstest(uint32_t rxBaseId, uint32_t txBaseId);
   virtual ~Stresstest();

   virtual void receiveEvent(std::auto_ptr< ::tsd::communication::event::TsdEvent> event);
   virtual std::auto_ptr< ::tsd::communication::event::TsdEvent> deserialize(::tsd::common::ipc::RpcBuffer& buf);

   void start();
   void print();
   
private:
   const uint32_t m_RxBaseId;
   const uint32_t m_TxBaseId;
   std::map<uint32_t, std::vector<uint32_t> > m_ReceiveMap;
   std::map<uint32_t, std::vector<uint32_t> > m_ErrorMap;
};

#endif // _STRESSTEST_H_
