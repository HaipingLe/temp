///////////////////////////////////////////////////////
//!\file TsdEventSerializer.cpp
//!\brief Implementation of TsdEventSerializer class
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/communication/IEventSerializer.hpp>
#include <tsd/communication/TsdEventSerializer.hpp>

using tsd::communication::TsdEventSerializer;

TsdEventSerializer * TsdEventSerializer::m_spInstance = NULL;
tsd::common::system::Mutex TsdEventSerializer::m_sMutex;


TsdEventSerializer::TsdEventSerializer()
   : common::dispatch::Serializer<event::TsdEvent>()
{
   m_log = new tsd::common::logging::Logger("tsd.communication.serializer");
}

TsdEventSerializer::~TsdEventSerializer()
{
   delete m_log;
}

TsdEventSerializer * TsdEventSerializer::getInstance()
{
   if (m_spInstance == NULL) {
      tsd::common::system::MutexGuard locker(m_sMutex);
      if (m_spInstance == NULL) {
         m_spInstance = new TsdEventSerializer();
      }
   }

   return m_spInstance;
}

void TsdEventSerializer::serialize(tsd::common::ipc::RpcBuffer& buf, const tsd::communication::event::TsdEvent& obj)
{
   buf << obj.getEventId(); // So don't store m_EventId in impementations of TsdEvent
   obj.serialize(buf);
}

std::auto_ptr<tsd::communication::event::TsdEvent> TsdEventSerializer::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   std::auto_ptr<event::TsdEvent> ret;
   const uint32_t eventId = buf.peekInt();

   tsd::common::system::MutexGuard locker(m_serializersLock);

   /*
    * Don't care about the fact that we might grow the map for events that we
    * don't handle. Either it's an application error or a race condition when
    * deregistering events. Both are unlikely enough to just ignore them.
    */
   IEventSerializer *serializer = m_serializers[eventId];
   if (serializer) {
      ret.reset(serializer->deserialize(buf).release());
   } else {
      *m_log << tsd::common::logging::LogLevel::Warn
             << "No deserializer found for event 0x" << std::hex << eventId
             << std::endl;
   }

   return ret;
}

void TsdEventSerializer::addSerializer(tsd::communication::IEventSerializer *serializer,
                                       const std::vector<uint32_t>& events)
{
   if (!events.empty()) {
      addSerializer(serializer, &events[0], static_cast<uint32_t>(events.size()));
   }
}

void TsdEventSerializer::addSerializer(tsd::communication::IEventSerializer *serializer,
                                       const uint32_t *events,
                                       const uint32_t numEvents)
{
   tsd::common::system::MutexGuard locker(m_serializersLock);

   std::set<uint32_t> &serializerEvents = m_serializerEvents[serializer];

   for (uint32_t i = 0; i < numEvents; i++) {
      uint32_t id = events[i];
      if (m_serializers[id] != NULL) {
         *m_log << tsd::common::logging::LogLevel::Error
                << "Serializer already registered for event 0x" << std::hex << id
                << std::endl;
      }
      m_serializers[id] = serializer; // last serializer always wins
      m_allSerializers[id].insert(serializer);
      serializerEvents.insert(id);
   }
}

void TsdEventSerializer::removeSerializer(tsd::communication::IEventSerializer *serializer)
{
   tsd::common::system::MutexGuard locker(m_serializersLock);

   std::set<uint32_t> &events = m_serializerEvents[serializer];
   for (std::set<uint32_t>::iterator it(events.begin()); it != events.end(); ++it) {
      uint32_t event = *it;

      /*
       * We don't shrink the maps because it's just not worth the code and
       * runtime.
       */
      std::set<IEventSerializer *> &eventSerializers = m_allSerializers[event];
      eventSerializers.erase(serializer);
      if (eventSerializers.empty()) {
         m_serializers[event] = NULL;
      } else {
         m_serializers[event] = *eventSerializers.begin();
      }
   }

   m_serializerEvents.erase(serializer);
}

