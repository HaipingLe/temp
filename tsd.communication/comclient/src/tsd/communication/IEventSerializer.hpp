///////////////////////////////////////////////////////
//!\file IEventSerializer.hpp
//!\brief Declaration of the interface IEventSerializer
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_IEVENTSERIALIZER_HPP
#define TSD_COMMUNICATION_IEVENTSERIALIZER_HPP

#include <memory>
#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication {

class IEventSerializer
{
public:
   virtual ~IEventSerializer();

   //! Deserialize object from buffer.
   //! \param buffer [in] Buffer containing serialized object
   //! return             Deserialized object
   virtual std::auto_ptr< ::tsd::communication::event::TsdEvent> deserialize(common::ipc::RpcBuffer& buf) = 0;
};

}/* namespace communication */ } /* namespace tsd */

#endif
