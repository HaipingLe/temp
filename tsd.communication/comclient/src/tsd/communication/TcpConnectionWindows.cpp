//////////////////////////////////////////////////////////////////////
//!\file TcpConnectionWindows.cpp
//!\brief TODO: definition of file TcpConnectionWindows.cpp
//!\author d.wassenberg@technisat.de
//!
//!Copyright (c) TechniSat Digital GmbH
//!CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <fcntl.h>
#include <queue>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>

#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/TcpConnectionWindows.hpp>
#include <tsd/common/errors/ConnectException.hpp>
#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/MutexGuard.hpp>

namespace tsd {
namespace communication {
namespace client {

class TcpSendThread : public tsd::common::system::Thread {
public:
   TcpSendThread(TcpConnection &connection, SOCKET socket);
   ~TcpSendThread();

   virtual void run();

   bool send(Buffer *buf);

private:
   TcpConnection &m_connection;
   SOCKET m_socket;
   bool m_running;
   tsd::common::system::Mutex m_queueLock;
   tsd::common::system::Semaphore m_queueSem;
   std::queue<Buffer*> m_queue;

   uint8_t *m_SendBuffer;
   uint32_t m_SendBufferSize;
};

class TcpRecvThread : public tsd::common::system::Thread {
public:
   TcpRecvThread(TcpConnection &connection, SOCKET socket);
   ~TcpRecvThread();

   virtual void run();

private:
   TcpConnection &m_connection;
   SOCKET m_socket;
   bool m_running;

   uint8_t *m_ReceiveBuffer;
   uint32_t m_ReceiveBufferSize;
   uint32_t m_inPtr;
   uint32_t m_outPtr;
};

}
}
}

using tsd::communication::client::TcpRecvThread;
using tsd::communication::client::TcpSendThread;
using tsd::communication::client::TcpConnection;
using namespace tsd::communication::client;

namespace {

template<typename T>
T readUnaligned(void *buf) {
   if (reinterpret_cast<uintptr_t>(buf) & (sizeof(T) - 1)) {
      // unaligned
      T tmp;
      std::memcpy(&tmp, buf, sizeof(T));
      return tmp;
   }
   else {
      // aligned
      return *(T*) buf;
   }
}

}

TcpRecvThread::TcpRecvThread(TcpConnection &connection, SOCKET socket) :
         tsd::common::system::Thread("tsd.communication.comclient.tcp.recv"),
         m_connection(connection),
         m_socket(socket),
         m_running(false),
         m_ReceiveBuffer(NULL),
         m_ReceiveBufferSize(524288 + 4) {
   m_ReceiveBuffer = static_cast<uint8_t*>(std::malloc(m_ReceiveBufferSize));
   m_inPtr = m_outPtr = 0;

   if (m_ReceiveBuffer) {
      m_running = true;

      start();
   }
   else {
      m_running = false;
   }
}

TcpRecvThread::~TcpRecvThread() {
   if (m_running) {
      m_running = false;

      if (m_socket != INVALID_SOCKET) {
         shutdown(m_socket, SD_RECEIVE);
      }
      join();
   }

   if (m_ReceiveBuffer) {
      std::free(m_ReceiveBuffer);
   }
}

void TcpRecvThread::run() {
   m_connection.m_log << tsd::common::logging::LogLevel::Trace << "entering main loop" << std::endl;

   while (m_running) {
      m_connection.m_log << tsd::common::logging::LogLevel::Trace << "loop" << std::endl;

      SSIZE_T readBytes;

      readBytes = recv(m_socket, (char*) &m_ReceiveBuffer[m_inPtr], m_ReceiveBufferSize - m_inPtr, 0);

      m_connection.m_log << tsd::common::logging::LogLevel::Trace << "received " << readBytes << " bytes data" << std::endl;

      if (m_running) {
         if (readBytes == 0) {
            m_connection.m_log << tsd::common::logging::LogLevel::Warn << "TcpRecvThread read failed because socket closed!" << std::endl;

            m_connection.disconnected();
            break;
         }
         else if (readBytes < 0) {
            m_connection.m_log << tsd::common::logging::LogLevel::Warn << "TcpRecvThread read failed because of error code "
            << WSAGetLastError() << std::endl;

            m_connection.disconnected();
            break;
         }
         else if (readBytes == SOCKET_ERROR) {
            m_connection.m_log << tsd::common::logging::LogLevel::Warn << "TcpRecvThread read failed (SOCKET_ERROR) because of error code "
            << WSAGetLastError() << std::endl;

            m_connection.disconnected();
            break;
         }
         else if (readBytes == INVALID_SOCKET) {
            m_connection.m_log << tsd::common::logging::LogLevel::Warn
            << "TcpRecvThread read failed (INVALID_SOCKET) because of error code " << WSAGetLastError() << std::endl;

            m_connection.disconnected();
            break;
         }
         else {
            m_inPtr += readBytes;

            while (m_inPtr > m_outPtr + 4) {
               uint32_t msgLen = readUnaligned<uint32_t>(&m_ReceiveBuffer[m_outPtr]);

               // can we hold the whole message?
               if (m_outPtr + 4 + msgLen > m_ReceiveBufferSize) {
                  if ((m_outPtr > 0) && (4 + msgLen <= m_ReceiveBufferSize)) {
                     // move to the front
                     std::memmove(m_ReceiveBuffer, &m_ReceiveBuffer[m_outPtr], m_inPtr - m_outPtr);
                  }
                  else {
                     // reallocate buffer because it's too small
                     while (m_ReceiveBufferSize < 4 + msgLen)
                        m_ReceiveBufferSize *= 2;

                     uint8_t *newBuffer = static_cast<uint8_t*>(std::malloc(m_ReceiveBufferSize));
                     std::memcpy(newBuffer, &m_ReceiveBuffer[m_outPtr], m_inPtr - m_outPtr);
                     std::free(m_ReceiveBuffer);
                     m_ReceiveBuffer = newBuffer;
                  }

                  m_inPtr -= m_outPtr;
                  m_outPtr = 0;
                  continue;
               }

               // do we have a full message yet?
               if (m_outPtr + 4 + msgLen > m_inPtr) {
                  break;
               }

               m_connection.received(&m_ReceiveBuffer[m_outPtr + 4], msgLen);
               m_outPtr += 4 + msgLen;
               if (m_outPtr == m_inPtr)
                  m_outPtr = m_inPtr = 0;
            }
         }
      }
   }
}

TcpSendThread::TcpSendThread(TcpConnection &connection, SOCKET socket) :
         tsd::common::system::Thread("tsd.communication.comclient.tcp.send"),
         m_connection(connection),
         m_socket(socket),
         m_running(false),
         m_queueLock(),
         m_queueSem(0),
         m_queue(),
         m_SendBuffer(NULL),
         m_SendBufferSize(524288 + 4) {
   m_SendBuffer = static_cast<uint8_t*>(std::malloc(m_SendBufferSize));

   if (m_SendBuffer) {
      m_running = true;
      start();
   }
   else {
      m_running = false;
   }
}

TcpSendThread::~TcpSendThread() {
   if (m_running) {
      m_running = false;

      if (m_socket != INVALID_SOCKET) {
         shutdown(m_socket, SD_SEND);
      }

      m_queueSem.up();

      join();
   }

   while (!m_queue.empty()) {
      m_queue.front()->deref();
      m_queue.pop();
   }

   if (m_SendBuffer) {
      std::free(m_SendBuffer);
   }
}

void TcpSendThread::run() {
   m_connection.m_log << tsd::common::logging::LogLevel::Trace << "entering main loop" << std::endl;

   while (m_running) {
      Buffer* msg = NULL;

      m_connection.m_log << tsd::common::logging::LogLevel::Trace << "loop" << std::endl;

      /* wait until something is in the queue */
      m_queueSem.down();

      m_connection.m_log << tsd::common::logging::LogLevel::Trace << "received something to send" << std::endl;

      m_queueLock.lock();

      if (!m_queue.empty()) {
         msg = m_queue.front();
         m_queue.pop();
      }

      m_queueLock.unlock();

      if (msg) {
         memset(m_SendBuffer, 0, m_SendBufferSize);
         ((uint32_t*) m_SendBuffer)[0] = msg->length();
         memcpy((void*) ((uint32_t) m_SendBuffer + sizeof(uint32_t)), msg->payload(), msg->length());

         SSIZE_T written = 0;
         SSIZE_T pendingBytes = msg->lengthWithHeader();
         uint8_t* pendingData = m_SendBuffer;

         do {
            written = ::send(m_socket, (char*) pendingData, pendingBytes, 0);

            pendingBytes -= written;
            pendingData = (uint8_t*) (((uint32_t) pendingData) + written);

            m_connection.m_log << tsd::common::logging::LogLevel::Trace << "send data: successfully send: " << written << ", pending: "
            << pendingBytes << std::endl;
         }
         while (written >= 0 && pendingBytes > 0);

         if (written <= 0 && pendingBytes > 0) {
            if (m_running) {
               if (written == INVALID_SOCKET) {
                  m_connection.m_log << tsd::common::logging::LogLevel::Warn << "TcpSendThread write failed because socket closed!"
                                     << std::endl;
               }
               else if (written == SOCKET_ERROR) {
                  m_connection.m_log << tsd::common::logging::LogLevel::Warn
                  << "TcpRecvThread read failed (SOCKET_ERROR) because of error code " << WSAGetLastError() << std::endl;
               }
               else {
                  m_connection.m_log << tsd::common::logging::LogLevel::Warn << "TcpRecvThread read failed because of error code "
                  << WSAGetLastError() << std::endl;
               }

               m_connection.disconnected();
               break;
            }

         }
         msg->deref();
      }
   }

   if (m_socket != INVALID_SOCKET) {
      shutdown(m_socket, SD_SEND);
   }

   m_running = false;
}

bool TcpSendThread::send(tsd::communication::Buffer *buf) {
   if (m_running) {
      buf->ref();

      m_queueLock.lock();
      m_queue.push(buf);
      m_queueLock.unlock();

      m_queueSem.up();
   }

   return m_running;
}

TcpConnection::TcpConnection(tsd::communication::client::IReceiveCallback *cb, tsd::common::logging::Logger &log, const std::string & name) :
         Connection(cb, log),
         m_socket(NULL),
         m_recvThread(NULL),
         m_sendThread(NULL),
         m_disconnectLock() {
   
   m_socket = INVALID_SOCKET;
   const char* service = "8911";
   std::string ip;

   struct addrinfo hints;
   struct addrinfo* res = NULL;

   memset(&hints, 0, sizeof(hints));

   hints.ai_family   = PF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if(name.length() <= 0)
   {
      ip = "127.0.0.1";
   }
   else
   {
      ip = name;
   }

   if (getaddrinfo(ip.c_str(), service, &hints, &res) == 0) {
      for (struct addrinfo* p = res; p != 0; p = p->ai_next) {
         SOCKET currentSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
         if (currentSocket != INVALID_SOCKET) {
            if (connect(currentSocket, res->ai_addr, res->ai_addrlen) == 0) {
               m_socket = currentSocket;
               break;
            }
            else
            {
               m_log << tsd::common::logging::LogLevel::Warn << "Failed to connect to communication manager because of error code "
                     << WSAGetLastError() << std::endl;
            }

            closesocket(currentSocket);
            m_socket = INVALID_SOCKET;
         }
         else
         {
            m_log << tsd::common::logging::LogLevel::Warn << "Failed to create socket because of error code "
                  << WSAGetLastError() << std::endl;
         }
      }

      freeaddrinfo(res);
   }

   if(m_socket != INVALID_SOCKET) {
      u_long arg = 1;
      ioctlsocket(m_socket, TCP_NODELAY, &arg);

      m_sendThread = new TcpSendThread(*this, m_socket);
      m_recvThread = new TcpRecvThread(*this, m_socket);
   }
   else {
      throw tsd::common::errors::SystemException("FAILED to connect to Communication Manager!");
   }
}

TcpConnection::TcpConnection(tsd::communication::client::IReceiveCallback *cb, tsd::common::logging::Logger &log, SOCKET socket) :
         Connection(cb, log),
         m_socket(socket) {
   m_sendThread = new TcpSendThread(*this, m_socket);
   m_recvThread = new TcpRecvThread(*this, m_socket);
}

TcpConnection::~TcpConnection() {
   delete m_recvThread;
   delete m_sendThread;

   if (m_socket != INVALID_SOCKET) {
      closesocket(m_socket);
   }
}

bool TcpConnection::send(const void *buf, uint32_t len) {
   Buffer *buffer = allocBuffer(len);
   buffer->fill(buf, len);
   bool ret = m_sendThread->send(buffer);
   buffer->deref();

   return ret;
}

bool TcpConnection::send(tsd::communication::Buffer *buf) {
   return m_sendThread->send(buf);
}

void TcpConnection::disconnected() {
   bool callback = false;

   tsd::common::system::MutexGuard guard(m_disconnectLock);

   if (m_socket != INVALID_SOCKET) {

      closesocket(m_socket);
      m_socket = INVALID_SOCKET;
      callback = true;
   }

   guard.unlock();

   if (callback) {
      Connection::disconnected();
   }
}

