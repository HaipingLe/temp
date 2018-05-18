
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <tsd/common/errors/SystemException.hpp>
#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/Client.hpp>
#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/communication/TcpBackend.hpp>
#include <tsd/communication/TcpConnection.hpp>

namespace tsd { namespace communication {

   class TcpClient : public Client
                   , public client::IReceiveCallback
   {
   public:
      TcpClient(TcpBackend &backend, int fd);

      virtual void pushMessage(Buffer *msg); // Client
      virtual void messageReceived(const void *buf, uint32_t len); // IReceiveCallback
      virtual void disconnected(); // IReceiveCallback

   private:
      TcpBackend &m_backend;
      client::TcpConnection m_connection;
   };

}}

namespace {

   void setBlocking(int fd)
   {
      int flags = fcntl(fd, F_GETFL, 0);
      fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
   }

   void setNonBlocking(int fd)
   {
      int flags = fcntl(fd, F_GETFL, 0);
      fcntl(fd, F_SETFL, flags | O_NONBLOCK);
   }

   void setTcpNoDelay(int fd)
   {
      int opt = 1;
      setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
   }
}

using tsd::communication::TcpBackend;
using tsd::communication::TcpClient;

TcpBackend::TcpBackend(CommunicationManager &cm,
                       tsd::common::logging::Logger &logger,
                       const std::string & address)
   : Thread("tsd.communication.commgr.tcp")
   , m_cm(cm)
   , m_log(logger)
{
   struct sockaddr_in addr;

   std::memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = static_cast<in_port_t>(htons(8911));
   addr.sin_addr.s_addr = htonl(INADDR_ANY);

   if (address.length() > 0) {
      std::string::size_type colon = address.find(":");
      if (colon != std::string::npos) {
         addr.sin_port = static_cast<in_port_t>(htons(
               static_cast<uint16_t>(std::atol(address.substr(colon+1).c_str())) ));
      }

      std::string host(address.substr(0, colon));
      if (inet_aton(host.c_str(), &addr.sin_addr) == 0) {
         std::string msg("Invalid host address: ");
         msg += host;
         throw tsd::common::errors::SystemException(msg);
      }
   }

   m_socket = socket(AF_INET, SOCK_STREAM, 0);
   if (m_socket < 0) {
      throw tsd::common::errors::SystemException("socket failed");
   }

   if (bind(m_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
      close(m_socket);
      throw tsd::common::errors::SystemException("bind failed");
   }

   if (listen(m_socket, 4) < 0) {
      close(m_socket);
      throw tsd::common::errors::SystemException("listen failed");
   }

   if (pipe(m_wakeupPipe) < 0) {
      close(m_socket);
      throw tsd::common::errors::SystemException("pipe failed");
   }

   setNonBlocking(m_wakeupPipe[0]);
   setNonBlocking(m_wakeupPipe[1]);
   setNonBlocking(m_socket);

   m_fds[0].fd = m_wakeupPipe[0];
   m_fds[0].events = POLLIN;
   m_fds[1].fd = m_socket;
   m_fds[1].events = POLLIN;

   m_running = true;
   start();
}

TcpBackend::~TcpBackend()
{
   uint8_t dummy = 0;

   m_running = false;
   write(m_wakeupPipe[1], &dummy, sizeof(dummy));
   join();

   close(m_socket);
   close(m_wakeupPipe[0]);
   close(m_wakeupPipe[1]);
}

void TcpBackend::run()
{
   m_cm.lock();

   while (m_running) {
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
      int ret = poll(m_fds, 2, -1);
      m_cm.lock();

      if (ret == -1) {
         if (errno == EINTR)
            continue;

         int err = errno;
         m_log << tsd::common::logging::LogLevel::Error << "poll " << err
               << std::endl;
         break;
      }

      if (m_fds[0].revents) {
         handleWakeup(m_fds[0].revents);
      }
      if (m_fds[1].revents) {
         handleConnect(m_fds[1].revents);
      }
   }

   for (std::set<TcpClient*>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
      delete *it;
   }

   m_cm.unlock();
}

void TcpBackend::handleWakeup(short /*pollEvents*/)
{
   char buf[16];
   ssize_t len;

   do {
      len = read(m_wakeupPipe[0], buf, sizeof(buf));
   } while (len < 0 && errno == EINTR);
}

void TcpBackend::handleConnect(short pollEvents)
{
   if (pollEvents & POLLIN) {
      int fd;
      do {
         fd = accept(m_socket, NULL, NULL);
      } while (fd < 0 && errno == EINTR);

      if (fd >= 0) {
         setBlocking(fd);
         setTcpNoDelay(fd);

         TcpClient *client = new TcpClient(*this, fd);
         m_clients.insert(client);
         m_cm.registerClient(client);
      } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != ECONNABORTED) {
         int err = errno;
         m_log << tsd::common::logging::LogLevel::Warn << "accept failed: " << err
               << std::endl;
      }
   }

   if (pollEvents & POLLERR) {
      /* FIXME */
   }
}

void TcpBackend::handleDisconnect(TcpClient *client)
{
   m_cleanupQueueLock.lock();
   m_cleanupQueue.push(client);
   m_cleanupQueueLock.unlock();

   // wakeup main loop to dispose client
   uint8_t dummy = 0;
   write(m_wakeupPipe[1], &dummy, sizeof(dummy));
}


TcpClient::TcpClient(TcpBackend &backend, int fd)
   : m_backend(backend)
   , m_connection(this, backend.m_log, fd)
{
}

void TcpClient::pushMessage(Buffer *msg)
{
   m_connection.send(msg);
}

void TcpClient::messageReceived(const void *buf, uint32_t len)
{
   Buffer *msg = allocBuffer(len);
   msg->fill(buf, len);

   m_backend.m_cm.lock();
   m_backend.m_cm.dispatchMessage(this, msg);
   m_backend.m_cm.unlock();
}

void TcpClient::disconnected()
{
   m_backend.handleDisconnect(this);
}
