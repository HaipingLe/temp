///////////////////////////////////////////////////////
//!\file CommunicationClient.hpp
//!\brief CommunicationClient definition
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////
#ifndef TSD_COMMUNICATION_COMMUNICATIONCLIENT_HPP
#define TSD_COMMUNICATION_COMMUNICATIONCLIENT_HPP

#include <list>
#include <map>
#include <set>
#include <vector>

#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/types/typedef.hpp>
#include <tsd/communication/Connection.hpp>
#include <tsd/communication/ICommunicationClient.hpp>
#include <tsd/communication/TsdEventSerializer.hpp>

/* FIXME: workaround for waveplayer. remove later... */
#include <cstdio>

namespace tsd { namespace common { namespace logging {
   class Logger;
} } }


namespace tsd { namespace communication {

class IComReceive;
class ComClientQueue;

//////////////////////////////////////////////////////////////////////
//! \class  CommunicationClient
//! \author Oliver Flatau
//! \date   2011/03/15
//! \brief  Definition of the class CommunicationClient
//!				
//! CommunicationClient is an active controller class (singleton) and realize the functionality to
//! communicate with the CommunicationManager. CommunicationClient realize the
//! interface ICommunicationClient. Users have to work with ICommunicationClient.
//!
//////////////////////////////////////////////////////////////////////
class CommunicationClient : public ICommunicationClient
   , client::IReceiveCallback
{
private:
   //! Default constructor
   CommunicationClient(void);
   //! Destructor
   virtual ~CommunicationClient(void);

public:
   //! Initialise the connection to the Communication Manager
   //! You only can send and receive events after you call this function
   //! \param ReceiverName [in] Name of this Client
   //! \param pIP [in] IP address of Communication Manager. NULL to use shared memory. "LOCAL" to assume that CM
   //! is in our address space or already connected.
   virtual void init(const std::string& ReceiverName, const char_t* pIP = NULL, bool implicitConnect = false);

   virtual IComClientQueue* getQueue(const std::string &name, QueueType type);

   //! Sends the given event to CommuncationManager
   //! \param tsdevent [in] Event
   //! \return Result of send
   virtual bool send(std::auto_ptr<tsd::communication::event::TsdEvent> tsdevent);

   //! Register a class that implements IComReceive to receive events from CommunicationManager
   //! \param observer [in] Pointer of class to be registered
   //! \param events [in]   List of events that the registered class will receive
   //! \return              Result of registration
   virtual bool addObserver(IComReceive* observer, const std::vector<uint32_t>& events);

   //! Register a class that implements IComReceive to receive events from CommunicationManager
   //! The supplied array with pEvents needs be valid even after the call of addObserver.
   //! So don't delete or modify it.
   //! \param observer [in]  Pointer of class to be registered
   //! \param pEvents [in]   pointer to event array that the registered class will receive
   //! \param numEvents [in] size of event array pointed to by pEvents (number of events contained)
   //! \return               Result of registration
   virtual bool addObserver(IComReceive* observer, const uint32_t * pEvents, const uint32_t numEvents);

   //! Unregister a class that implements IComReceive to receive events from CommunicationManager
   //! \param observer [in] Pointer of class to be unregistered
   //! \return              Result of unregistration
   virtual bool deleteObserver(IComReceive* observer);

   //! Get access of CommunicationClient singleton. Instanciate 
   //! the singleton if it's not already done.
   static CommunicationClient* getInstance(void);

   //! Destroy CommunicationClient singleton
   static void destroyInstance(void);


   enum DebugFunc {
      SET_DOGGY         = 0, /* Deprecated. Has no effect... */
      DISABLE_RECEIVE   = 1, /* disables receiving events of a other channel. param2 pointer to the channel name char string */
      DISABLE_TRANSMIT  = 2, /* disables trasmitting events of a other channel. param2 pointer to the channel name char string */
   };

   //! Enter Debug-Mode, for example disabling Doggy, DON'T RELY on it in release software.
   //! \param Which debug function we want
   //! \param param1 first param for the debug function
   //! \param param2 second param for the debug function
   //! \return true if supported, false if not or error
   bool setDebugMode(DebugFunc type, uint32_t param1, void * param2);

   bool registerQueueEvents(ComClientQueue *queue, const std::vector<uint32_t> &events);
   bool deregisterQueueEvents(ComClientQueue *queue, const std::vector<uint32_t> &events);
   void deregisterQueue(ComClientQueue *queue);
   void watchdogExpired(const std::string &name, uint32_t id, uint32_t timeEntered,
      uint32_t timeStarted, uint32_t timeWd, uint32_t now);

   virtual void messageReceived(const void *buf, uint32_t len);
   virtual void disconnected();

   inline tsd::common::logging::Logger* getLogger()
   {
      return m_Log;
   }

private:
   static CommunicationClient*       s_Instance;    //!< the singleton object
   static tsd::common::system::Mutex s_InstanceMux; //!< singleton protector

   client::Connection *m_Connection;
   tsd::communication::TsdEventSerializer* m_Serializer;
   tsd::common::system::Mutex m_InitMux;
   std::map<std::string, uint32_t> m_queueTimeouts;

   uint32_t *m_SndBuffer;
   uint32_t m_SndBufferSize;
   tsd::common::system::Mutex m_SndBufferMux;

   typedef std::map< uint32_t, std::vector<ComClientQueue*> > tQueues;
   tQueues m_Queues;
   tsd::common::system::Mutex m_QueuesLock;
   ComClientQueue *m_defaultQueue;
   typedef std::map<std::string, ComClientQueue*> tAllQueues;
   tAllQueues m_allQueues;

   tsd::common::logging::Logger* m_Log;

   //! forbidden copy constructor
   CommunicationClient(CommunicationClient const &);

   //! forbidden assignment operator
   CommunicationClient& operator = (CommunicationClient const &);

   void notify(std::auto_ptr<event::TsdEvent> event);
   uint32_t getQueueTimeout(const std::string &name, QueueType type) const;
};

} /* namespace communication */ } /* namespace tsd */


#endif // TSD_COMMUNICATION_COMMUNICATIONCLIENT_HPP
