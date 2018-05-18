///////////////////////////////////////////////////////
//!\file ICommunicationClient.hpp
//!\brief Declaration of the interface ICommunicationClient
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_ICOMMUNICATIONCLIENT_HPP
#define TSD_COMMUNICATION_ICOMMUNICATIONCLIENT_HPP

#include <tsd/common/types/typedef.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

namespace std
{
   template<class _Ty> class auto_ptr;
}

namespace tsd { namespace communication { 

class IComReceive;

/**
 * Communication client queue.
 *
 * Events registered at a queue are dispatched in a dedicated thread. Use
 * ICommunicationClient::getQueue() to create/get such a queue.
 *
 * Be carefull when destroying a queue. Although you can just delete them make
 * sure that you do not interfere with any other thread.
 */
class IComClientQueue
{
public:
   virtual ~IComClientQueue();

   virtual bool addObserver(IComReceive* observer, const std::vector<uint32_t>& events) = 0;
   virtual bool addObserver(IComReceive* observer, const uint32_t * pEvents, const uint32_t numEvents) = 0;
   virtual bool deleteObserver(IComReceive* observer) = 0;
};

//////////////////////////////////////////////////////////////////////
//! \class  ICommunicationClient
//! \author Oliver Flatau, Rene Reusner
//! \date   2011/03/15
//! \brief  Definition of the interface ICommunicationClient
//!				
//! ICommunicationClient defines all shared functions of 
//! the CommunicationClient singleton.
//!
//////////////////////////////////////////////////////////////////////
class TSD_COMMUNICATION_COMCLIENT_DLLEXPORT ICommunicationClient
{
public:
   //! Destructor
   virtual ~ICommunicationClient(void);
   
   //! Get access of CommunicationClient singleton. Instanciate 
   //! the singleton if it's not already done.
   static ICommunicationClient* getInstance(void);

   //! singleton registration - to prevent instanciating the default implementation
   //! \param pInstance [in] pointer to singleton instance to be registered
   //! \return               whether the registration was successful
   static bool registerInstance(ICommunicationClient * pInstance);
   
   //! \deprecated Don't use init from interface ICommunicationClient! it should only be called one time
   //!             in the main function of a process by the init function of derived class CommunicationClient!
   DEPRECATED virtual void init(const std::string& ReceiverName, const char_t* pIP = NULL, bool implicitConnect = true) = 0;

   /**
    * Supported queue types.
    *
    * The queue type should express the workload of the queue.
    */
   enum QueueType {
      RealtimeQueue, // user input stuff (default 2s timeout)
      NormalQueue,   // normal events (default 10s timeout)
      BulkQueue,     // Bulk processing (default 30s timeout)
      LongRunQueue   // heavy lifting (default 2min timeout)
   };

   /**
    * Get an named, dedicated queue.
    *
    * If a queue with this name does not exist it will be implicitly created.
    * Subsequent calls with the same name will return the same queue.
    *
    * Observers registered for events on that queue will be dispatched
    * independent of other queues. The CommunicationClient always has a
    * default queue named "default" which will be used if
    * addObserver()/deleteObserver() is called on the ICommunicationClient
    * interface.
    *
    * \note Queue names should be kept short to keep the overhead low.
    *
    * \param name A short name for the queue. Used to find the queue and to
    *             identify the dispatching thread in the system.
    * \param type Queue type. Effectively determines the watchdog timeout for
    *             events.
    */
   virtual IComClientQueue* getQueue(const std::string &name, QueueType type) = 0;

   //! Sends the given event to CommuncationManager
   //! \param tsdevent [in] Event
   //! \return Result of send
   virtual bool send(std::auto_ptr<tsd::communication::event::TsdEvent> tsdevent) = 0;

   //! Register a class that implements IComReceive to receive events from CommunicationManager
   //! \param observer [in]  Pointer of class to be registered
   //! \param events [in]    List of events that the registered class will receive
   //! \return               Result of registration
   virtual bool addObserver(IComReceive* observer, const std::vector<uint32_t>& events) = 0;
      
   //! Register a class that implements IComReceive to receive events from CommunicationManager
   //! \param observer [in]  Pointer of class to be registered
   //! \param pEvents [in]   pointer to event array that the registered class will receive WARNING! - this pointer must
   //!                       still be valid after this call, reading is deferrred
   //! \param numEvents [in] size of event array pointed to by pEvents (number of events contained)
   //! \return               Result of registration
   virtual bool addObserver(IComReceive* observer, const uint32_t * pEvents, const uint32_t numEvents) = 0;

   //! Unregister a class that implements IComReceive to receive events from CommunicationManager
   //! \param observer [in] Pointer of class to be unregistered
   //! \return              Result of unregistration
   virtual bool deleteObserver(IComReceive* observer) = 0;

private:
   //! singleton pointer
   static ICommunicationClient * sm_pInstance;
};

} /* namespace communication */ } /* namespace tsd */

#endif // TSD_COMMUNICATION_ICOMMUNICATIONCLIENT_HPP
