
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <set>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <tsd/common/assert.hpp>
#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>

#include "IPort.hpp"
#include "Packet.hpp"
#include "Router.hpp"
#include "Select.hpp"
#include "SocketHelper.hpp"
#include "TcpEndpoint.hpp"
#include "TcpServerPort.hpp"

namespace {

   void setTcpNoDelay(int fd)
   {
      int opt = 1;
      setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
   }

}

namespace tsd { namespace communication { namespace messaging {

   class TcpServerPort::Impl
      : private tsd::common::system::Thread
   {
      class ListenSocket
         : public ISelectEventHandler
      {
         enum SocketType {
            ST_UNKNOWN,       // not initialized
            ST_TCP,           // TCP socket
            ST_UNIX,          // classic unix socket
            ST_UNIX_ABSTRACT  // Linux unix socket in abstract namespace
         };

         TcpServerPort::Impl &m_server;
         int m_socket;
         SocketType m_type;
         std::string m_unixName;

      public:
         ListenSocket(TcpServerPort::Impl &server);
         ~ListenSocket();

         bool selectReadable();  // ISelectEventHandler
         bool selectWritable();  // ISelectEventHandler
         bool selectError();     // ISelectEventHandler

         int getFd() const { return m_socket; }
         void initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf);
         void initUnix(const std::string &path);
         uint16_t getBoundPort();
         std::string getBoundPath();
      };

      class Client
         : public TcpEndpoint
         , public IPort
      {
         TcpServerPort::Impl &m_server;

      protected:
         void epReceivedPacket(std::auto_ptr<Packet> pkt);  // TcpEndpoint
         void epDisconnected();  // TcpEndpoint

         bool startPort(); // IPort
         void stopPort();  // IPort

      public:
         Client(TcpServerPort::Impl &server);
         ~Client();

         bool init(int fd, Select &selector);
         bool sendPacket(std::auto_ptr<Packet> pkt);  // IPort
      };

      void addClient(int fd);
      void delClient(Client *client);

      void run(); // tsd::common::system::Thread

      typedef std::set<Client*> ClientList;
      typedef std::deque<Client*> ClientQueue;

      Router &m_router;
      bool m_running;
      Select m_select;
      ClientList m_clients;
      tsd::common::system::Mutex m_lock;
      tsd::common::logging::Logger m_log;
      ListenSocket m_listenSocket;
      ClientQueue m_pendingDeletes;
      SelectSource* m_selectSource;

   public:
      Impl(Router &router);
      ~Impl();

      void disconnect();
      void initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf);
      void initUnix(const std::string &path);
      uint16_t getBoundPort()
      {
         return m_listenSocket.getBoundPort();
      }
      std::string getBoundPath()
      {
         return m_listenSocket.getBoundPath();
      }
   };

} } }

using tsd::communication::messaging::ConnectionException;
using tsd::communication::messaging::TcpServerPort;

TcpServerPort::Impl::ListenSocket::ListenSocket(TcpServerPort::Impl &server)
   : m_server(server)
   , m_socket(-1)
   , m_type(ST_UNKNOWN)
{
}

TcpServerPort::Impl::ListenSocket::~ListenSocket()
{
   if (m_socket != -1) {
      close(m_socket);
   }

   if (m_type == ST_UNIX) {
      // ignore errors removing the socket
      unlink(m_unixName.c_str());
   }
}

void TcpServerPort::Impl::ListenSocket::initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf)
{
   signal(SIGPIPE, SIG_IGN);

   struct sockaddr_in sockaddr;

   std::memset(&sockaddr, 0, sizeof(sockaddr));
   sockaddr.sin_family = AF_INET;
   sockaddr.sin_port = static_cast<in_port_t>(htons(port));
   sockaddr.sin_addr.s_addr = htonl(addr);

   m_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
   if (m_socket < 0) {
      int err = errno;
      throw ConnectionException(std::string("Could not create socket: ") +
         std::strerror(err));
   }
   m_type = ST_TCP;

   // Allow port reuse if building for test. Don't make unit tests fail on
   // TIME_WAIT left overs...
#ifdef TSD_BUILDENV_BUILDING_TEST
   int opt = 1;
   setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

   if (bind(m_socket, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
      int err = errno;
      close(m_socket);
      m_socket = -1;
      throw ConnectionException(std::string("Could not bind socket: ") +
         std::strerror(err));
   }

   if (sndBuf > 0) {
      int size = sndBuf;
      if (setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) != 0) {
         int err = errno;
         m_server.m_log << tsd::common::logging::LogLevel::Error
               << "Unable to set send buffer size: "
               << std::strerror(err) << std::endl;
      }
   }

   if (rcvBuf > 0) {
      int size = rcvBuf;
      if (setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != 0) {
         int err = errno;
         m_server.m_log << tsd::common::logging::LogLevel::Error
               << "Unable to set receive buffer size: "
               << std::strerror(err) << std::endl;
      }
   }

   if (listen(m_socket, 32) < 0) {
      int err = errno;
      close(m_socket);
      m_socket = -1;
      throw ConnectionException(std::string("Listen failed: ") + std::strerror(err));
   }
}

void TcpServerPort::Impl::ListenSocket::initUnix(const std::string &path)
{
   struct sockaddr_un sockaddr;
   socklen_t sockaddrLen;

   m_unixName = path;
   bool abstract = tsd::communication::messaging::parseUnixPath(path,
      sockaddr, sockaddrLen);

   signal(SIGPIPE, SIG_IGN);

   m_socket = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
   if (m_socket < 0) {
      int err = errno;
      throw ConnectionException(std::string("Could not create socket: ") +
         std::strerror(err));
   }

   if (bind(m_socket, (struct sockaddr *) &sockaddr, sockaddrLen) < 0) {
      int err = errno;
      close(m_socket);
      m_socket = -1;
      throw ConnectionException(std::string("Could not bind socket: ") +
         std::strerror(err));
   }

   // the socket will be visible _after_ bind succeeded
   m_type = abstract ? ST_UNIX_ABSTRACT : ST_UNIX;

   if (listen(m_socket, 32) < 0) {
      int err = errno;
      close(m_socket);
      m_socket = -1;
      throw ConnectionException(std::string("Listen failed: ") + std::strerror(err));
   }
}

uint16_t TcpServerPort::Impl::ListenSocket::getBoundPort()
{
   if (m_type == ST_TCP) {
      struct sockaddr_in addr;
      socklen_t addr_len = sizeof(addr);
      if (getsockname(m_socket, (struct sockaddr*) &addr, &addr_len) == -1) {
         return 0;
      } else {
         return ntohs(addr.sin_port);
      }
   } else {
      return 0;
   }
}

std::string TcpServerPort::Impl::ListenSocket::getBoundPath()
{
   struct sockaddr_un addr;
   socklen_t addr_len = sizeof(addr);

   std::string ret;

   if (m_type == ST_UNIX) {
      if (getsockname(m_socket, (struct sockaddr*) &addr, &addr_len) == 0) {
         ret = addr.sun_path;
      }
   } else if (m_type == ST_UNIX_ABSTRACT) {
      if (getsockname(m_socket, (struct sockaddr*) &addr, &addr_len) == 0) {
         ret = "@";
         ret.append(&addr.sun_path[1], addr_len-sizeof(sa_family_t)-1);
      }
   }

   return ret;
}

bool TcpServerPort::Impl::ListenSocket::selectReadable()
{
   int fd;

   do {
      do {
         fd = accept4(m_socket, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
      } while (fd == -1 && errno == EINTR);

      if (fd != -1) {
         if (m_type == ST_TCP) {
            setTcpNoDelay(fd);
         }
         m_server.addClient(fd);
      } else {
         // TODO: log error
      }
   } while (fd != -1 || (errno != EAGAIN && errno != EWOULDBLOCK));

   return true;
}

bool TcpServerPort::Impl::ListenSocket::selectWritable()
{
   return false; // should never be called
}

bool TcpServerPort::Impl::ListenSocket::selectError()
{
   // FIXME: shut down server?
   return false;
}

/*****************************************************************************/

TcpServerPort::Impl::Client::Client(TcpServerPort::Impl &server)
   : TcpEndpoint(server.m_log)
   , IPort(server.m_router)
   , m_server(server)
{
}

TcpServerPort::Impl::Client::~Client()
{
}

void TcpServerPort::Impl::Client::epReceivedPacket(std::auto_ptr<Packet> pkt)
{
   receivedPacket(pkt);
}

void TcpServerPort::Impl::Client::epDisconnected()
{
   m_server.delClient(this);
}

bool TcpServerPort::Impl::Client::startPort()
{
   return true;
}

void TcpServerPort::Impl::Client::stopPort()
{
}

bool TcpServerPort::Impl::Client::sendPacket(std::auto_ptr<Packet> pkt)
{
   return epSendPacket(pkt);
}

bool TcpServerPort::Impl::Client::init(int fd, Select &selector)
{
   if (!TcpEndpoint::init(fd, selector)) {
      return false;
   }

   if (!initDownstream()) {
      return false;
   }

   bool ret = connected();
   if (!ret) {
      finish();
   }

   return ret;
}

/*****************************************************************************/

TcpServerPort::Impl::Impl(tsd::communication::messaging::Router &router)
   : tsd::common::system::Thread("TcpServerPort")
   , m_router(router)
   , m_running(false)
   , m_log("tsd.communication.TcpServerPort")
   , m_listenSocket(*this)
   , m_selectSource(NULL)
{
}

TcpServerPort::Impl::~Impl()
{
   if (m_selectSource != NULL) {
      m_selectSource->dispose();
   }
}

void TcpServerPort::Impl::disconnect()
{
   tsd::common::system::MutexGuard g(m_lock);
   if (m_running) {
      m_running = false;
      g.unlock();
      m_select.interrupt();
      join();
   }
}

void TcpServerPort::Impl::initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf)
{
   if (!m_select.init()) {
      throw ConnectionException("Selector init failed!");
   }

   m_listenSocket.initV4(addr, port, sndBuf, rcvBuf);

   m_selectSource = m_select.add(m_listenSocket.getFd(), &m_listenSocket, true, false);
   if (m_selectSource == NULL) {
      throw ConnectionException("Could not monitor listen socket for read!");
   }

   m_running = true;
   start();
}

void TcpServerPort::Impl::initUnix(const std::string &path)
{
   if (!m_select.init()) {
      throw ConnectionException("Selector init failed!");
   }

   m_listenSocket.initUnix(path);

   m_selectSource = m_select.add(m_listenSocket.getFd(), &m_listenSocket, true, false);
   if (m_selectSource == NULL) {
      throw ConnectionException("Could not monitor listen socket for read!");
   }

   m_running = true;
   start();
}

void TcpServerPort::Impl::addClient(int fd)
{
   std::auto_ptr<Client> client(new Client(*this));
   bool ok = client->init(fd, m_select);
   if (ok) {
      m_clients.insert(client.release());
   } else {
      m_log << tsd::common::logging::LogLevel::Error
            << "Client init failed!" << &std::endl;
   }
}

void TcpServerPort::Impl::delClient(Client *client)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_pendingDeletes.push_back(client);
   g.unlock();

   m_select.interrupt();
}

void TcpServerPort::Impl::run()
{
   tsd::common::system::MutexGuard g(m_lock);

   while (m_running) {
      // dispatch and wait for interrupt
      g.unlock();
      m_select.dispatch();
      g.lock();

      // process pending lists
      while (!m_pendingDeletes.empty()) {
         Client *client = m_pendingDeletes.front();
         m_pendingDeletes.pop_front();
         m_clients.erase(client);

         // drop the lock when deleting the client because we up-call to the router
         g.unlock();
         client->finish();
         delete client;
         g.lock();
      }
   }

   // clean up all clients
   g.unlock();
   for (ClientList::iterator it(m_clients.begin()); it != m_clients.end(); ++it) {
      (*it)->finish();
      delete *it;
   }
}

/*****************************************************************************/

TcpServerPort::TcpServerPort(tsd::communication::messaging::Router &router)
   : m_p(new Impl(router))
{
}

TcpServerPort::~TcpServerPort()
{
   m_p->disconnect();
   delete m_p;
}

void TcpServerPort::disconnect()
{
   m_p->disconnect();
}

void TcpServerPort::initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf)
{
   m_p->initV4(addr, port, sndBuf, rcvBuf);
}

void TcpServerPort::initUnix(const std::string &path)
{
   m_p->initUnix(path);
}

uint16_t TcpServerPort::getBoundPort()
{
   return m_p->getBoundPort();
}

std::string TcpServerPort::getBoundPath()
{
   return m_p->getBoundPath();
}
