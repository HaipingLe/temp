
#include <cstdlib>
#include <cstring>

#include <tsd/common/ipc/rpcbuffer.h>

#include "Packet.hpp"

using tsd::communication::messaging::Packet;

Packet::Packet(const Packet &obj)
   : m_senderAddr(obj.m_senderAddr)
   , m_receiverAddr(obj.m_receiverAddr)
   , m_eventId(obj.m_eventId)
   , m_buffer(obj.m_buffer)
   , m_type(obj.m_type)
{
   // FIXME: reference count buffer instead of memcpy
}

Packet::Packet(tsd::communication::event::TsdEvent *msg, bool multicast)
   : m_senderAddr(msg->getSenderAddr())
   , m_receiverAddr(msg->getReceiverAddr())
   , m_eventId(msg->getEventId())
   , m_type(multicast ? MULTICAST_MESSAGE : UNICAST_MESSAGE)
{
   m_buffer.reserve(4096);

   tsd::common::ipc::RpcBuffer rpcBuf;
   rpcBuf.init(&m_buffer);
   msg->serialize(rpcBuf);
}

Packet::Packet(Type type, tsd::communication::event::IfcAddr_t sender,
               tsd::communication::event::IfcAddr_t receiver)
   : m_senderAddr(sender)
   , m_receiverAddr(receiver)
   , m_eventId(0)
   , m_type(type)
{
}

Packet::Packet(Type type, tsd::communication::event::IfcAddr_t sender,
               tsd::communication::event::IfcAddr_t receiver, uint32_t eventId,
               const char *buffer, uint32_t bufferLength)
   : m_senderAddr(sender)
   , m_receiverAddr(receiver)
   , m_eventId(eventId)
   , m_type(type)
{
   m_buffer.resize(bufferLength);
   std::memcpy(&(m_buffer[0]), buffer, bufferLength);
}

Packet::~Packet()
{
}

