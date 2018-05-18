///////////////////////////////////////////////////////
//!\file TcpConnection.cpp
//!\brief Implementation of the POSIX TCP connection
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <queue>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <tsd/common/assert.hpp>
#include <tsd/common/errors/ConnectException.hpp>
#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/TcpConnection.hpp>

namespace tsd { namespace communication { namespace client {

   class TcpSendThread : public tsd::common::system::Thread
   {
   public:
      TcpSendThread(TcpConnection &connection, int socket);
      ~TcpSendThread();

      virtual void run();

   bool send(Buffer *buf);

   private:
      TcpConnection &m_connection;
      int m_socket;
      volatile bool m_running;
      tsd::common::system::Mutex m_queueLock;
      tsd::common::system::Semaphore m_queueSem;
      std::queue<Buffer*> m_queue;
   };

   class TcpRecvThread : public tsd::common::system::Thread
   {
   public:
      TcpRecvThread(TcpConnection &connection, int socket);
      ~TcpRecvThread();

      virtual void run();

   private:
      TcpConnection &m_connection;
      int m_socket;
      volatile bool m_running;

      uint8_t *m_buffer;
      uint32_t m_bufferSize;
      uint32_t m_inPtr;
      uint32_t m_outPtr;
   };

}}}

using tsd::communication::client::TcpRecvThread;
using tsd::communication::client::TcpSendThread;
using tsd::communication::client::TcpConnection;

namespace {

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

}


TcpRecvThread::TcpRecvThread(TcpConnection &connection, int socket)
   : tsd::common::system::Thread("tsd.communication.comclient.tcp.recv")
   , m_connection(connection)
   , m_socket(socket)
{
   m_bufferSize = 32768;
   m_buffer = static_cast<uint8_t*>(std::malloc(m_bufferSize));
   ASSERT_FATAL(m_buffer != NULL, "Out of memory");
   m_inPtr = m_outPtr = 0;

   m_running = true;
   start();
}

TcpRecvThread::~TcpRecvThread()
{
   m_running = false;
   shutdown(m_socket, SHUT_RD);
   join();

   std::free(m_buffer);
}

void TcpRecvThread::run()
{
   while (m_running) {
      ssize_t len;
      do {
         len = read(m_socket, &m_buffer[m_inPtr], m_bufferSize - m_inPtr);
      } while (len < 0 && errno == EINTR);

      if (len <= 0) {
         if (m_running) {
            if (len < 0) {
               int err = errno;
               m_connection.m_log << tsd::common::logging::LogLevel::Warn
                                  << "TcpRecvThread read failed: "
                                  << std::strerror(err) << std::endl;
            }
            m_connection.disconnected();
         }
         break;
      }

      m_inPtr += static_cast<uint32_t>(len);
      while (m_inPtr > m_outPtr+4) {
         uint32_t msgLen = readUnaligned<uint32_t>(&m_buffer[m_outPtr]);

         // can we hold the whole message?
         if (m_outPtr+4+msgLen > m_bufferSize) {
            if ((m_outPtr > 0) && (4+msgLen <= m_bufferSize)) {
               // move to the front
               std::memmove(m_buffer, &m_buffer[m_outPtr], m_inPtr-m_outPtr);
            } else {
               // reallocate buffer because it's too small
               while (m_bufferSize < 4+msgLen)
                  m_bufferSize *= 2;

               uint8_t *newBuffer = static_cast<uint8_t*>(std::malloc(m_bufferSize));
               ASSERT_FATAL(newBuffer != NULL, "Out of memory");
               std::memcpy(newBuffer, &m_buffer[m_outPtr], m_inPtr-m_outPtr);
               std::free(m_buffer);
               m_buffer = newBuffer;
            }

            m_inPtr -= m_outPtr;
            m_outPtr = 0;
            continue;
         }

         // do we have a full message yet?
         if (m_outPtr+4+msgLen > m_inPtr)
            break;

         m_connection.received(&m_buffer[m_outPtr+4], msgLen);
         m_outPtr += 4 + msgLen;
         if (m_outPtr == m_inPtr)
            m_outPtr = m_inPtr = 0;
      }
   }
}


#define MAX_COALESCE 8

TcpSendThread::TcpSendThread(TcpConnection &connection, int socket)
   : tsd::common::system::Thread("tsd.communication.comclient.tcp.send")
   , m_connection(connection)
   , m_socket(socket)
   , m_queueSem(0)
{
   m_running = true;
   start();
}

TcpSendThread::~TcpSendThread()
{
   m_running = false;
   shutdown(m_socket, SHUT_WR);
   m_queueSem.up();
   join();

   while (!m_queue.empty()) {
      m_queue.front()->deref();
      m_queue.pop();
   }
}

void TcpSendThread::run()
{
   // FIXME: fix excessive loops if coalescing happens
   while (m_running) {
      struct iovec iov[MAX_COALESCE * 2];
      uint32_t headers[MAX_COALESCE];
      Buffer *packets[MAX_COALESCE];
      uint32_t num = 0;
      int len = 0;

      m_queueLock.lock();
      while (!m_queue.empty() && num < MAX_COALESCE) {
         Buffer *msg = packets[num] = m_queue.front();
         m_queue.pop();

         headers[num] = msg->length();
         iov[num*2 + 0].iov_base = &headers[num];
         iov[num*2 + 0].iov_len  = 4;
         iov[num*2 + 1].iov_base = msg->payload();
         iov[num*2 + 1].iov_len  = msg->length();

         len += 4 + msg->length();
         num++;
      }
      m_queueLock.unlock();

      if (num) {
         ssize_t written;
         struct iovec *pending_iov = iov;
         uint32_t pending_num = num*2;

         do {
            do {
               written = writev(m_socket, pending_iov, pending_num);
            } while (written < 0 && errno == EINTR);

            if (written > 0) {
               if (written < len) {
                  size_t pos = written;
                  // Partial write. Eat up done iovecs...
                  while (pos) {
                     if (pos >= pending_iov->iov_len) {
                        pos -= pending_iov->iov_len;
                        pending_num--;
                        pending_iov++;
                     } else {
                        pending_iov->iov_len -= pos;
                        pending_iov->iov_base = (void*)((uint8_t*)pending_iov->iov_base + pos);
                        pos = 0;
                     }
                  }
               }
               len -= static_cast<int>(written);
            }
         } while (written > 0 && len > 0);

         if (written <= 0) {
            if (m_running) {
               if (written < 0) {
                  int err = errno;
                  m_connection.m_log << tsd::common::logging::LogLevel::Warn
                                     << "TcpSendThread write failed: "
                                     << std::strerror(err) << std::endl;
               }
               m_connection.disconnected();
            }
            break;
         }

         for (uint32_t i = 0; i < num; i++) {
            packets[i]->deref();
         }
      }

      /* wait until something is in the queue */
      m_queueSem.down();
   }

   shutdown(m_socket, SHUT_WR);
   m_running = false;
}

bool TcpSendThread::send(Buffer *buf)
{
   if (m_running) {
      buf->ref();

      m_queueLock.lock();
      m_queue.push(buf);
      m_queueLock.unlock();

      m_queueSem.up();
   }

   return m_running;
}


TcpConnection::TcpConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log,
                             const std::string & name)
   : Connection(cb, log)
{
   signal(SIGPIPE, SIG_IGN);

   m_socket = socket(AF_INET, SOCK_STREAM, 0);
   if (m_socket < 0) {
      throw tsd::common::errors::SystemException("socket failed");
   }

   struct sockaddr_in addr;

   std::memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = static_cast<in_port_t>(htons(8911));
   addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

   if (name.length() > 0) {
      std::string::size_type colon = name.find(":");
      if (colon != std::string::npos) {
         addr.sin_port = static_cast<in_port_t>(htons(
               static_cast<uint16_t>(std::atol(name.substr(colon+1).c_str())) ));
      }

      std::string host(name.substr(0, colon));
      if (inet_aton(host.c_str(), &addr.sin_addr) == 0) {
         std::string msg("Invalid host address: ");
         msg += host;
         throw tsd::common::errors::SystemException(msg);
      }
   }

   int ret;
   do {
      ret = connect(m_socket, (struct sockaddr *) &addr, sizeof(addr));
   } while (ret < 0 && errno == EINTR);
   if (ret < 0) {
      std::stringstream s;
      s << "connect failed: " << std::strerror(errno);

      close(m_socket);
      throw tsd::common::errors::SystemException(s.str());
   }

   int opt = 1;
   setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

   m_sendThread = new TcpSendThread(*this, m_socket);
   m_recvThread = new TcpRecvThread(*this, m_socket);
}

TcpConnection::TcpConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log,
                             int socket)
   : Connection(cb, log)
   , m_socket(socket)
{
   signal(SIGPIPE, SIG_IGN);

   m_sendThread = new TcpSendThread(*this, m_socket);
   m_recvThread = new TcpRecvThread(*this, m_socket);
}

TcpConnection::~TcpConnection()
{
   delete m_recvThread;
   delete m_sendThread;

   if (m_socket > 0) {
      close(m_socket);
   }
}

bool TcpConnection::send(const void *buf, uint32_t len)
{
   Buffer *buffer = allocBuffer(len);
   buffer->fill(buf, len);
   bool ret = m_sendThread->send(buffer);
   buffer->deref();

   return ret;
}

bool TcpConnection::send(Buffer *buf)
{
   return m_sendThread->send(buf);
}

void TcpConnection::disconnected()
{
   bool callback = false;

   tsd::common::system::MutexGuard guard(m_disconnectLock);

   if (m_socket > 0) {
      close(m_socket);
      m_socket = -1;
      callback = true;
   }

   guard.unlock();

   if (callback) {
      Connection::disconnected();
   }
}

