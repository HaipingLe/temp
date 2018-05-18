//////////////////////////////////////////////////////////////////////
//!\file ICommunicationClient.cpp
//!\brief Implementation of class ICommunicationClient
//!
//!\author rene.reusner@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/communication/ICommunicationClient.hpp>
#include <tsd/communication/CommunicationClient.hpp>

namespace tsd { namespace communication { 

namespace
{
   // local Mutex object - could as well be a static member of the class, will not clog up include dependencies here
   ::tsd::common::system::Mutex sMutex;
}

IComClientQueue::~IComClientQueue()
{
}

// singleton pointer
ICommunicationClient * ICommunicationClient::sm_pInstance = NULL;
    
// destructor
ICommunicationClient::~ICommunicationClient(void)
{
}

// constructor
ICommunicationClient* ICommunicationClient::getInstance(void)
{
   if (sm_pInstance == NULL)
   {
      ::tsd::common::system::MutexGuard lock(sMutex);
      if (sm_pInstance == NULL)
      {
         sm_pInstance = CommunicationClient::getInstance();
      }
   }
   return sm_pInstance;
}

// singleton registration
bool ICommunicationClient::registerInstance(ICommunicationClient * pInstance)
{
   bool ret = false;
   if (sm_pInstance == NULL)
   {
      ::tsd::common::system::MutexGuard lock(sMutex);
      if (sm_pInstance == NULL)
      {
         sm_pInstance = pInstance;
         ret = true;
      }
   }
   return ret;
}

} /* namespace communication */ } /* namespace tsd */
