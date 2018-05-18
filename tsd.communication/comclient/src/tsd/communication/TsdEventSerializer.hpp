///////////////////////////////////////////////////////
//!\file TsdEventSerializer.hpp
//!\brief Declaration of TsdEventSerializer class
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_TSDEVENTSERIALIZER_HPP
#define TSD_COMMUNICATION_TSDEVENTSERIALIZER_HPP

#include <map>
#include <set>

#include <tsd/common/dispatch/serializer.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/types/typedef.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace common { namespace logging {
   class Logger;
} } }

namespace tsd { namespace communication {

class IEventSerializer;

//////////////////////////////////////////////////////////////////////
//! \class  TsdEventSerializer
//! \author Oliver Flatau, Stefan Zill
//! \date   2011/03/15
//! \brief  Definition of the class TsdEventSerializer
//!				
//! TsdEventSerializer implements the interface common::dispatch::Serializer
//! for serialization of all TsdEvent 
//!
//////////////////////////////////////////////////////////////////////
class TSD_COMMUNICATION_COMCLIENT_DLLEXPORT TsdEventSerializer : public common::dispatch::Serializer<event::TsdEvent>
{
private:
   TsdEventSerializer();
   virtual ~TsdEventSerializer();

   static TsdEventSerializer * m_spInstance;
   static tsd::common::system::Mutex m_sMutex;

public:
   //! singleton instance getter
   static TsdEventSerializer * getInstance();

   //! Serialize object into buffer (interface implementation)
   //! \param buffer [out] Buffer to receive serialized object
   //! \param object [in]  Object to serialize
   virtual void serialize(common::ipc::RpcBuffer& buf, const event::TsdEvent& obj);

   //! Deserialize object from buffer.
   //! \param buffer [in] Buffer containing serialized object
   //! return             Deserialized object
   virtual std::auto_ptr<event::TsdEvent> deserialize(common::ipc::RpcBuffer& buf);

   virtual void addSerializer(IEventSerializer * pSerializer, const std::vector<uint32_t>& eventIds);
   virtual void addSerializer(IEventSerializer * pSerializer, const uint32_t * pEventIds, const uint32_t numEventIds);
   virtual void removeSerializer(IEventSerializer * pSerializer);

private:
   tsd::common::system::Mutex m_serializersLock;
   std::map<uint32_t, IEventSerializer *> m_serializers;
   std::map<uint32_t, std::set<IEventSerializer *> > m_allSerializers;
   std::map<IEventSerializer *, std::set<uint32_t> >  m_serializerEvents;
   tsd::common::logging::Logger *m_log;
};

} /* namespace communication */ } /* namespace tsd */

#endif // TSD_COMMUNICATION_TSDEVENTSERIALIZER_HPP
