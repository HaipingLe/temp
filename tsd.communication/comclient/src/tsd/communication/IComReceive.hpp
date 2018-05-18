///////////////////////////////////////////////////////
//!\file ICommunicationClient.hpp
//!\brief Declaration of the interface IComReceive
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_ICOMRECEIVE_HPP
#define TSD_COMMUNICATION_ICOMRECEIVE_HPP

#include <tsd/communication/event/TsdEvent.hpp>
#include <memory>

namespace tsd { namespace communication { 

//////////////////////////////////////////////////////////////////////
//! \class  IComReceive
//! \author Oliver Flatau
//! \date   2011/03/15
//! \brief  Declaration of the interface IComReceive
//!				
//! IComReceive can be implemented bei classes to receive Events from the 
//! CommunicationClient.
//!
//////////////////////////////////////////////////////////////////////
class IComReceive
{
public:
   //! Destructor
   virtual ~IComReceive(void) {}

   //! Process a received event
   //! \param event [in] Received event
   virtual void receiveEvent(std::auto_ptr< ::tsd::communication::event::TsdEvent> event) = 0;
};

} /* namespace communication */ } /* namespace tsd */

#endif
