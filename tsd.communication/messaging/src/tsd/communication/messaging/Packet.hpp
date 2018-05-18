#ifndef TSD_COMMUNICATION_MESSAGING_SERIALIZEDEVENT_HPP
#define TSD_COMMUNICATION_MESSAGING_SERIALIZEDEVENT_HPP

#include <vector>

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace messaging {

// TODO: add TTL!
class Packet
{
public:
   // WARNING: do not change codes! The numbers are transmitted over networks.
   enum Type {
      UNICAST_MESSAGE      = 0,
      MULTICAST_JOIN       = 1,
      MULTICAST_LEAVE      = 2,
      MULTICAST_MESSAGE    = 3,
      DEATH_NOTIFICATION   = 4,
      DHCP_OFFER           = 5,

      OOB_BASE             = 128,   // OOB data is used by the transports
   };

   Packet(const Packet &obj); // copy constructor
   Packet(tsd::communication::event::TsdEvent *msg, bool multicast);
   Packet(Type type, tsd::communication::event::IfcAddr_t sender,
      tsd::communication::event::IfcAddr_t receiver);
   Packet(Type type, tsd::communication::event::IfcAddr_t sender,
      tsd::communication::event::IfcAddr_t receiver, uint32_t eventId,
      const char *buffer, uint32_t bufferLength);
   ~Packet();

   inline Type getType() const
   {
      return m_type;
   }

   inline tsd::communication::event::IfcAddr_t getSenderAddr() const
   {
      return m_senderAddr;
   }

   inline void setSenderAddr(tsd::communication::event::IfcAddr_t addr)
   {
      m_senderAddr = addr;
   }

   inline tsd::communication::event::IfcAddr_t getReceiverAddr() const
   {
      return m_receiverAddr;
   }

   inline void setReceiverAddr(tsd::communication::event::IfcAddr_t addr)
   {
      m_receiverAddr = addr;
   }

   inline uint32_t getEventId() const
   {
      return m_eventId;
   }

   inline size_t getBufferLength() const
   {
      return m_buffer.size();
   }

   inline char *getBufferPtr()
   {
      return &(m_buffer[0]);
   }

private:
   tsd::communication::event::IfcAddr_t m_senderAddr;
   tsd::communication::event::IfcAddr_t m_receiverAddr;
   uint32_t m_eventId;
   std::vector<char> m_buffer;
   Type m_type;

   // not assignable
   Packet& operator=(const Packet &);
};

} } }

#endif
