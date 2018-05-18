#include <errno.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#include <tsd/common/assert.hpp>
#include <tsd/common/ipc/networkinteger.h>

#include "Packet.hpp"
#include "TcpEndpoint.hpp"

using tsd::communication::messaging::TcpEndpoint;
using tsd::common::ipc::NetworkInteger;

namespace {

   const ssize_t PARTIAL_PACKET_DONE = -1;
   const ssize_t PARTIAL_PACKET_DISCONNECTED = -2;

   template<typename T>
   T readUnaligned(void *buf)
   {
      if (reinterpret_cast<uintptr_t>(buf) & (sizeof(T)-1)) {
         // unaligned
         T tmp;
         std::memcpy(&tmp, buf, sizeof(T));
         return tmp;
      } else {
         // aligned
         return *(T*)buf;
      }
   }

   /**
    * Eat up IO vector by @p skip bytes.
    */
   void advanceIOV(struct iovec * &iov, int &iov_num, size_t skip)
   {
      while (skip > 0 && iov_num > 0) {
         if (skip >= iov->iov_len) {
            skip -= iov->iov_len;
            iov++;
            iov_num--;
         } else {
            iov->iov_len -= skip;
            iov->iov_base = (void*)((char*)iov->iov_base + skip);
            skip = 0;
         }
      }
   }

}

#define HEADER_SIZE  (  4 + /* msgLen */        \
                        4 + /* eventId */       \
                        8 + /* senderAddr */    \
                        8 + /* receiverAddr */  \
                        1 + /* type */          \
                        3   /* padding */       \
                     )

struct PacketHeader {
   NetworkInteger<uint32_t>   m_msgLen;
   NetworkInteger<uint32_t>   m_eventId;
   NetworkInteger<uint64_t>   m_senderAddr;
   NetworkInteger<uint64_t>   m_receiverAddr;
   uint8_t                    m_type;
   uint8_t                    m_padding[3];
};

TcpEndpoint::TcpEndpoint(tsd::common::logging::Logger &log)
   : m_log(log)
   , m_socket(-1)
   , m_selectSource(NULL)
   , m_sendOffset(0)
   , m_alive(false)
{
   m_bufferSize = 32768;
   m_buffer = static_cast<char*>(std::malloc(m_bufferSize));
   ASSERT_FATAL(m_buffer != NULL, "Out of memory");
   m_inPtr = m_outPtr = 0;
}

TcpEndpoint::~TcpEndpoint()
{
   cleanup();
   while (!m_sendQueue.empty()) {
      delete m_sendQueue.front();
      m_sendQueue.pop_front();
   }
   std::free(m_buffer);
}

bool TcpEndpoint::init(int fd, Select &selector)
{
   m_socket = fd;
   m_alive = true;
   m_selectSource = selector.add(fd, this);
   return m_selectSource != NULL;
}

void TcpEndpoint::cleanup()
{
   m_alive = false;

   if (m_selectSource != NULL) {
      m_selectSource->dispose();
      m_selectSource = NULL;
   }

   if (m_socket != -1) {
      shutdown(m_socket, SHUT_RDWR);
      close(m_socket);
      m_socket = -1;
   }
}

void TcpEndpoint::setDisconnected()
{
   tsd::common::system::MutexGuard g(m_lock);
   if (m_alive) {
      m_alive = false;
      g.unlock();
      epDisconnected();
   }
}

bool TcpEndpoint::selectReadable()
{
   ssize_t len;
   bool didReceive, ret = true;

   do {
      didReceive = false;

      // read new data
      do {
         len = read(m_socket, &m_buffer[m_inPtr], m_bufferSize - m_inPtr);
      } while (len < 0 && errno == EINTR);

      // check if remote end has disconnected
      if (len <= 0) {
         if (len == 0) {
            setDisconnected();
            ret = false;
         } else if (len < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            int err = errno;
            m_log << tsd::common::logging::LogLevel::Warn
                  << "TcpEndpoint read failed: "
                  << std::strerror(err) << std::endl;
            setDisconnected();
            ret = false;
         }
      } else {
         m_inPtr += len;
      }

      // dissect what we have
      uint32_t msgLen = 0;
      while (m_inPtr-m_outPtr >= sizeof(uint32_t)) {
         msgLen = readUnaligned<uint32_t>(&m_buffer[m_outPtr]) + HEADER_SIZE; // FIXME: NetworkInteger

         // do we have a full message yet?
         if (m_inPtr-m_outPtr < msgLen)
            break;

         received(&m_buffer[m_outPtr]);
         m_outPtr += msgLen;
         didReceive = true;
      }

      // allocate bigger buffer or, at least, move everything to front of buffer
      if (msgLen > m_bufferSize) {
         // reallocate buffer because it's too small
         while (m_bufferSize < msgLen)
            m_bufferSize *= 2;

         char *newBuffer = static_cast<char*>(std::malloc(m_bufferSize));
         ASSERT_FATAL(newBuffer != NULL, "Out of memory");
         std::memcpy(newBuffer, &m_buffer[m_outPtr], m_inPtr-m_outPtr);
         std::free(m_buffer);
         m_buffer = newBuffer;
         m_inPtr -= m_outPtr;
         m_outPtr = 0;
      } else if (m_outPtr == m_inPtr) {
         m_outPtr = m_inPtr = 0;
      } else if (m_outPtr > 0) {
         // move to the front
         std::memmove(m_buffer, &m_buffer[m_outPtr], m_inPtr-m_outPtr);
         m_inPtr -= m_outPtr;
         m_outPtr = 0;
      }
   } while (len > 0 || didReceive);

   return ret;
}

bool TcpEndpoint::selectWritable()
{
   bool ret = true;
   tsd::common::system::MutexGuard g(m_lock);

   while (!m_sendQueue.empty()) {
      Packet *pkt = m_sendQueue.front();
      ssize_t offset = sendPartialPacket(pkt, m_sendOffset);
      if (offset == PARTIAL_PACKET_DONE) {
         delete pkt;
         m_sendQueue.pop_front();
         m_sendOffset = 0;
      } else if (offset == PARTIAL_PACKET_DISCONNECTED) {
         g.unlock();
         setDisconnected();
         ret = false;
      } else {
         m_sendOffset = offset;
         break;
      }
   }

   return ret;
}

bool TcpEndpoint::selectError()
{
   m_log << tsd::common::logging::LogLevel::Warn
         << "TcpEndpoint: socket error state!"
         << std::endl;

   setDisconnected();
   return false;
}

bool TcpEndpoint::epSendPacket(std::auto_ptr<Packet> pkt)
{
   bool ret = true;
   tsd::common::system::MutexGuard g(m_lock);

   if (m_sendQueue.empty()) {
      ssize_t offset = sendPartialPacket(pkt.get(), 0);
      if (offset >= 0) {
         // The socket was full. This will trigger a selectWritable() once
         // there is enough room.
         m_sendQueue.push_back(pkt.release());
         m_sendOffset = offset;
      } else if (offset == PARTIAL_PACKET_DISCONNECTED) {
         g.unlock();
         setDisconnected();
         ret = false;
      }
   } else {
      m_sendQueue.push_back(pkt.release());
   }

   return ret;
}

void TcpEndpoint::received(const char *buffer)
{
   const PacketHeader *hdr = reinterpret_cast<const PacketHeader *>(buffer);
   Packet *pkt = new Packet(static_cast<tsd::communication::messaging::Packet::Type>(hdr->m_type),
                            hdr->m_senderAddr, hdr->m_receiverAddr,
                            hdr->m_eventId, buffer + HEADER_SIZE,
                            hdr->m_msgLen);

   epReceivedPacket(std::auto_ptr<Packet>(pkt));
}

ssize_t TcpEndpoint::sendPartialPacket(Packet *pkt, size_t offset)
{
   if (!m_alive) {
      // drop packet
      return PARTIAL_PACKET_DONE;
   }

   // construct IOV
   struct iovec iov[2];
   int iovcnt = 0;

   PacketHeader hdr;
   std::memset(&hdr, 0, sizeof(hdr));
   hdr.m_msgLen = static_cast<uint32_t>(pkt->getBufferLength());
   hdr.m_eventId = pkt->getEventId();
   hdr.m_senderAddr = pkt->getSenderAddr();
   hdr.m_receiverAddr = pkt->getReceiverAddr();
   hdr.m_type = pkt->getType();
   iov[iovcnt].iov_base = &hdr;
   iov[iovcnt].iov_len  = HEADER_SIZE;
   iovcnt++;

   if (pkt->getBufferLength() > 0) {
      iov[iovcnt].iov_base = pkt->getBufferPtr();
      iov[iovcnt].iov_len  = pkt->getBufferLength();
      iovcnt++;
   }

   // skip already sent data
   struct iovec *pending_iov = iov;
   int pending_num = iovcnt;
   advanceIOV(pending_iov, pending_num, offset);

   // put as much data into the socket as possible
   ssize_t written;
   do {
      do {
         written = writev(m_socket, pending_iov, pending_num);
      } while (written < 0 && errno == EINTR);

      if (written > 0) {
         advanceIOV(pending_iov, pending_num, written);
         offset += written;
      }
   } while (written > 0 && pending_num > 0);

   if (written < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      int err = errno;
      m_log << tsd::common::logging::LogLevel::Warn
            << "TcpEndpoint write failed: " << std::strerror(err)
            << std::endl;
      return PARTIAL_PACKET_DISCONNECTED;
   }

   return (pending_num > 0) ? offset : PARTIAL_PACKET_DONE;
}

