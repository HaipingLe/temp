//////////////////////////////////////////////////////////////////////
//!\file TcpBackendWindows.cpp
//!\brief TODO: definition of file TcpBackendWindows.cpp
//!\author d.wassenberg@technisat.de
//!
//!Copyright (c) TechniSat Digital GmbH
//!CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include <tsd/common/errors/SystemException.hpp>
#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/Client.hpp>
#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/communication/TcpBackendWindows.hpp>
#include <tsd/communication/TcpConnectionWindows.hpp>

#include <ws2tcpip.h>

namespace tsd {
namespace communication {

class TcpClient : public Client,
                  public client::IReceiveCallback {
public:
   TcpClient(TcpBackend &backend, int fd);

   virtual void pushMessage(Buffer *msg);  // Client
   virtual void messageReceived(const void *buf, uint32_t len);  // IReceiveCallback
   virtual void disconnected();  // IReceiveCallback

private:
   TcpBackend &m_backend;
   client::TcpConnection m_connection;
};

class TcpBackendListener : public tsd::common::system::Thread {
public:
   TcpBackendListener(TcpBackend& backend, const std::string & address);
   virtual ~TcpBackendListener();

   virtual void run();

protected:

private:
   SOCKET m_socket;
   TcpBackend& m_Backend;

};

}
}

namespace {

void setBlocking(SOCKET socket) {
   u_long arg = 0;
   ioctlsocket(socket, FIONBIO, &arg);
}

void setNonBlocking(SOCKET socket) {
   u_long arg = 1;
   ioctlsocket(socket, FIONBIO, &arg);
}

void setTcpNoDelay(SOCKET socket) {
   u_long arg = 1;
   ioctlsocket(socket, TCP_NODELAY, &arg);
}
}

using tsd::communication::TcpBackend;
using tsd::communication::TcpClient;
using tsd::communication::TcpBackendListener;
using namespace tsd::communication;
using namespace tsd::communication::client;

TcpBackend::TcpBackend(CommunicationManager &cm, tsd::common::logging::Logger &logger, const std::string & address) :
         Thread("tsd.communication.commgr.tcp"),
         m_Listener(NULL),
         m_cm(cm),
         m_log(logger),
         m_WaitSema(0) {
   m_running = true;

   m_Listener = new TcpBackendListener(*this, address);

   start();
   m_Listener->start();
}

TcpBackend::~TcpBackend() {
   m_running = false;

   //TODO: exit accept thread

   join();

   if (m_Listener) {
      m_Listener->join();
      delete m_Listener;
   }

   WSACleanup();
}

void TcpBackend::run() {
   m_log << tsd::common::logging::LogLevel::Trace << "entering main loop" << std::endl;

   while (m_running) {
      m_WaitSema.down();

      m_log << tsd::common::logging::LogLevel::Trace << "received event" << std::endl;

      m_cm.lock();
      m_cleanupQueueLock.lock();

      while (!m_cleanupQueue.empty()) {
         TcpClient *client = m_cleanupQueue.front();
         m_cleanupQueueLock.unlock();

         m_clients.erase(client);
         m_cm.deregisterClient(client);
         delete client;

         m_cleanupQueueLock.lock();
         m_cleanupQueue.pop();
      }
      m_cleanupQueueLock.unlock();

      m_cm.unlock();
   }

   m_cm.lock();

   for (std::set<TcpClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
      delete *it;
   }

   m_cm.unlock();
}

void TcpBackend::handleConnect(SOCKET socket) {
   m_cm.lock();

   TcpClient *client = new TcpClient(*this, socket);

   m_clients.insert(client);
   m_cm.registerClient(client);

   m_cm.unlock();
}

void TcpBackend::handleDisconnect(TcpClient *client) {
   m_cleanupQueueLock.lock();
   m_cleanupQueue.push(client);
   m_cleanupQueueLock.unlock();

   // wakeup main loop to dispose client
   m_WaitSema.up();
}

TcpClient::TcpClient(TcpBackend &backend, int fd) :
         m_backend(backend),
         m_connection(this, backend.m_log, fd) {
}

void TcpClient::pushMessage(Buffer *msg) {
   m_connection.send(msg);
}

void TcpClient::messageReceived(const void *buf, uint32_t len) {
   Buffer *msg = allocBuffer(len);
   msg->fill(buf, len);

   m_backend.m_cm.lock();
   m_backend.m_cm.dispatchMessage(this, msg);
   m_backend.m_cm.unlock();
}

void TcpClient::disconnected() {
   m_backend.handleDisconnect(this);
}

TcpBackendListener::TcpBackendListener(TcpBackend& backend, const std::string & address) :
         tsd::common::system::Thread("tsd.communication.commgr.tcplistener"),
         m_Backend(backend) {

   m_socket = INVALID_SOCKET;
   const char* ip = NULL;
   const char* service = "8911";

   struct addrinfo hints;
   struct addrinfo* res = NULL;

   memset(&hints, 0, sizeof(hints));

   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_flags = AI_PASSIVE;

   if(!address.empty())
   {
      ip = address.c_str();
   }

   if (getaddrinfo(ip, service, &hints, &res) != 0) {
      std::stringstream errorCode;
      errorCode << "failed to get address info because of error code ";
      errorCode << WSAGetLastError();
      errorCode << "!";

      freeaddrinfo(res);
      closesocket(m_socket);
      m_socket = INVALID_SOCKET;

      throw tsd::common::errors::SystemException(errorCode.str());
   }

   SOCKET currentSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   if (currentSocket == INVALID_SOCKET) {
      std::stringstream errorCode;
      errorCode << "failed to create socket because of error code ";
      errorCode << WSAGetLastError();
      errorCode << "!";

      freeaddrinfo(res);
      closesocket(m_socket);
      m_socket = INVALID_SOCKET;

      throw tsd::common::errors::SystemException(errorCode.str());
   }

   if (bind(currentSocket, res->ai_addr, (int) res->ai_addrlen) == SOCKET_ERROR) {
      std::stringstream errorCode;
      errorCode << "bind failed because of error code ";
      errorCode << WSAGetLastError();
      errorCode << "!";

      freeaddrinfo(res);
      closesocket(m_socket);
      m_socket = INVALID_SOCKET;

      throw tsd::common::errors::SystemException(errorCode.str());
   }

   m_socket = currentSocket;
   freeaddrinfo(res);

   if (listen(m_socket, 4) == SOCKET_ERROR) {
      std::stringstream errorCode;
      errorCode << "listen failed because of error code ";
      errorCode << WSAGetLastError();
      errorCode << "!";

      closesocket(m_socket);
      m_socket = INVALID_SOCKET;

      throw tsd::common::errors::SystemException(errorCode.str());
   }
}

TcpBackendListener::~TcpBackendListener() {
   if (m_socket != INVALID_SOCKET) {
      closesocket(m_socket);
//      m_socket = INVALID_SOCKET;
   }
}

void TcpBackendListener::run() {
   while (m_Backend.m_running) {
      SOCKET clientSocket = accept(m_socket, NULL, NULL);

      setTcpNoDelay(clientSocket);

      if (clientSocket == INVALID_SOCKET) {
         m_Backend.m_log << tsd::common::logging::LogLevel::Error << "accept failed because of error code " << WSAGetLastError() << "!"
         << std::endl;
      }
      else {
         m_Backend.m_log << tsd::common::logging::LogLevel::Trace << "accept successful!" << std::endl;

         m_Backend.handleConnect(clientSocket);
      }
   }
}

