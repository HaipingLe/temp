
#include <cstring>

#include <tsd/common/ipc/networkinteger.h>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/MutexGuard.hpp>

#include <tsd/communication/messaging/Connection.hpp>

#include "ConnectionImpl.hpp"
#include "IPort.hpp"
#include "Packet.hpp"
#include "Router.hpp"


using tsd::communication::messaging::ConnectionImpl;

/*****************************************************************************/

ConnectionImpl::ConnectionImpl(IConnectionCallbacks &cb, Router &router)
   : IPort(router)
   , m_cb(cb)
   , m_connected(false)
   , m_outgoingPkt(0)
   , m_outgoingOffset(0)
   , m_incomingHdrSize(0)
{
}

ConnectionImpl::~ConnectionImpl()
{
   purgeQueue();
}

void ConnectionImpl::nextPacket()
{
   if (m_outgoingPkt != 0) {
      delete m_outgoingPkt;
      m_outgoingPkt = 0;
      m_outgoingOffset = 0;
   }

   if (!m_outgoingQueue.empty()) {
      m_outgoingPkt = m_outgoingQueue.front();
      m_outgoingQueue.pop_front();

      std::memset(&m_outgoingHdr, 0, sizeof(m_outgoingHdr));
      m_outgoingHdr.m_msgLen = static_cast<uint32_t>(m_outgoingPkt->getBufferLength());
      m_outgoingHdr.m_eventId = m_outgoingPkt->getEventId();
      m_outgoingHdr.m_senderAddr = m_outgoingPkt->getSenderAddr();
      m_outgoingHdr.m_receiverAddr = m_outgoingPkt->getReceiverAddr();
      m_outgoingHdr.m_type = m_outgoingPkt->getType();
   }
}

void ConnectionImpl::processPacket(const uint8_t *p)
{
   Packet *pkt = new Packet(static_cast<tsd::communication::messaging::Packet::Type>(m_incomingHdr.m_type),
                            m_incomingHdr.m_senderAddr,
                            m_incomingHdr.m_receiverAddr,
                            m_incomingHdr.m_eventId,
                            reinterpret_cast<const char *>(p),
                            m_incomingHdr.m_msgLen);

   receivedPacket(std::auto_ptr<Packet>(pkt));
}

void ConnectionImpl::purgeQueue()
{
   m_incomingHdrSize = 0;
   m_incomingPkt.clear();

   tsd::common::system::MutexGuard g(m_lock);

   while (!m_outgoingQueue.empty()) {
      delete m_outgoingQueue.front();
      m_outgoingQueue.pop_front();
   }

   if (m_outgoingPkt != 0) {
      delete m_outgoingPkt;
      m_outgoingPkt = NULL;
   }

   m_outgoingOffset = 0;
}

bool ConnectionImpl::startPort()
{
   purgeQueue();
   return m_cb.setupConnection();
}

void ConnectionImpl::stopPort()
{
   m_cb.teardownConnection();
   purgeQueue();
}

bool ConnectionImpl::sendPacket(std::auto_ptr<Packet> pkt)
{
   tsd::common::system::MutexGuard g(m_lock);

   bool ret = false;

   if (m_connected) {
      bool wake = false;

      m_outgoingQueue.push_back(pkt.release());
      if (m_outgoingPkt == 0) {
         nextPacket();
         wake = true;
      }

      if (wake) {
         g.unlock();
         m_cb.wakeup();
      }
      ret = true;
   }

   return ret;
}

void ConnectionImpl::setConnected()
{
   tsd::common::system::MutexGuard g(m_lock);
   m_connected = true;
   g.unlock();

   bool ok = connected();

   if (!ok) {
      g.lock();
      m_connected = false;
   }
}

void ConnectionImpl::setDisconnected()
{
   disconnected();
   tsd::common::system::MutexGuard g(m_lock);
   m_connected = false;
   purgeQueue();
}

void ConnectionImpl::processData(const void* data, size_t size)
{
   const uint8_t* p = static_cast<const uint8_t*>(data);

   while (size > 0) {
      if (m_incomingHdrSize < sizeof(PacketHeader)) {
         // we always need a full header first
         size_t remaining = std::min(sizeof(PacketHeader) - m_incomingHdrSize, size);
         std::memcpy(reinterpret_cast<uint8_t*>(&m_incomingHdr) + m_incomingHdrSize, p, remaining);
         m_incomingHdrSize += remaining; p += remaining; size -= remaining;
      }
      if (m_incomingHdrSize == sizeof(PacketHeader)) {
         if (m_incomingPkt.empty() && m_incomingHdr.m_msgLen <= size) {
            // fast path: the payload is completely in the buffer and we have no backlog
            processPacket(p);
            p += m_incomingHdr.m_msgLen;
            size -= m_incomingHdr.m_msgLen;
            m_incomingHdrSize = 0;
         } else {
            // slow path: accumulate packet
            m_incomingPkt.reserve(m_incomingHdr.m_msgLen);
            size_t remaining = std::min(static_cast<size_t>(m_incomingHdr.m_msgLen - m_incomingPkt.size()), size);
            m_incomingPkt.insert(m_incomingPkt.end(), p, p + remaining);
            p += remaining; size -= remaining;
            if (m_incomingPkt.size() >= m_incomingHdr.m_msgLen) {
               processPacket(&m_incomingPkt.front());
               m_incomingHdrSize = 0;
               m_incomingPkt.clear();
            }
         }
      }
   }
}

bool ConnectionImpl::getData(const void*& data, size_t& size)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_outgoingPkt == 0) {
      return false;
   }

   if (m_outgoingOffset < sizeof(PacketHeader)) {
      data = reinterpret_cast<uint8_t*>(&m_outgoingHdr) + m_outgoingOffset;
      size = sizeof(PacketHeader) - m_outgoingOffset;
   } else {
      size_t offset = m_outgoingOffset - sizeof(PacketHeader);
      data = m_outgoingPkt->getBufferPtr() + offset;
      size = m_outgoingPkt->getBufferLength() - offset;
   }

   return true;
}

void ConnectionImpl::consumeData(size_t amount)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_outgoingPkt != 0) {
      m_outgoingOffset += amount;
      if (m_outgoingOffset >= (sizeof(PacketHeader) + m_outgoingPkt->getBufferLength())) {
         nextPacket();
      }
   }
}

bool ConnectionImpl::connectUpstream(const std::string &subDomain)
{
   return initUpstream(subDomain);
}

bool ConnectionImpl::listenDownstream()
{
   return initDownstream();
}

void ConnectionImpl::disconnect()
{
   finish();
}
