
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>

#include "IPort.hpp"
#include "Packet.hpp"
#include "Router.hpp"
#include "Select.hpp"
#include "SocketHelper.hpp"
#include "TcpClientPort.hpp"
#include "TcpEndpoint.hpp"

using tsd::communication::messaging::ConnectionException;
using tsd::communication::messaging::TcpClientPort;

namespace {

   void setTcpNoDelay(int fd)
   {
      int opt = 1;
      setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
   }

   void setNonBlocking(int fd)
   {
      int flags = fcntl(fd, F_GETFL, 0);
      fcntl(fd, F_SETFL, flags | O_NONBLOCK);
   }

}

namespace tsd { namespace communication { namespace messaging {

   class TcpClientPort::Impl
      : private tsd::common::system::Thread
      , private TcpEndpoint
      , private IPort
   {
      void run(); // tsd::common::system::Thread
      void epReceivedPacket(std::auto_ptr<Packet> pkt);  // TcpEndpoint
      void epDisconnected();  // TcpEndpoint
      bool sendPacket(std::auto_ptr<Packet> pkt);  // IPort
      bool startPort(); // IPort
      void stopPort();  // IPort

      Router &m_router;
      std::string m_subDomain;
      bool m_running;
      Select m_select;
      tsd::common::logging::Logger m_log;
      tsd::common::system::Mutex m_lock;

   public:
      Impl(Router &router, const std::string &subDomain);
      ~Impl();

      void disconnect();
      void initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf);
      void initUnix(const std::string &path);
   };

} } }

/*****************************************************************************/

TcpClientPort::Impl::Impl(tsd::communication::messaging::Router &router,
                          const std::string &subDomain)
   : tsd::common::system::Thread("TcpClientPort")
   , TcpEndpoint(m_log)
   , tsd::communication::messaging::IPort(router)
   , m_router(router)
   , m_subDomain(subDomain)
   , m_running(false)
   , m_log("tsd.communication.TcpClientPort")
{
}

TcpClientPort::Impl::~Impl()
{
   TcpEndpoint::cleanup();
}

void TcpClientPort::Impl::disconnect()
{
   finish();
}

void TcpClientPort::Impl::initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf)
{
   signal(SIGPIPE, SIG_IGN);

   bool ok = m_select.init();
   if (!ok) {
      throw ConnectionException("Selector init failed!");
   }

   int sock = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
   if (sock < 0) {
      int err = errno;
      throw ConnectionException(std::string("Could not create socket: ") +
         std::strerror(err));
   }

   if (sndBuf > 0) {
      int size = sndBuf;
      if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) != 0) {
         int err = errno;
         m_log << tsd::common::logging::LogLevel::Error
               << "Unable to set send buffer size: "
               << std::strerror(err) << std::endl;
      }
   }

   if (rcvBuf > 0) {
      int size = rcvBuf;
      if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != 0) {
         int err = errno;
         m_log << tsd::common::logging::LogLevel::Error
               << "Unable to set receive buffer size: "
               << std::strerror(err) << std::endl;
      }
   }

   struct sockaddr_in sockaddr;

   std::memset(&sockaddr, 0, sizeof(sockaddr));
   sockaddr.sin_family = AF_INET;
   sockaddr.sin_port = static_cast<in_port_t>(htons(port));
   sockaddr.sin_addr.s_addr = htonl(addr);

   int ret;
   do {
      ret = connect(sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
   } while (ret < 0 && errno == EINTR);
   if (ret < 0) {
      int err = errno;
      close(sock);
      throw ConnectionException(std::string("connect failed: ") +
         std::strerror(err));
   }

   setTcpNoDelay(sock);
   setNonBlocking(sock);

   if (!TcpEndpoint::init(sock, m_select)) {
      throw ConnectionException("TcpEndpoint init failed");
   }

   if (!initUpstream(m_subDomain)) {
      throw ConnectionException("initUpstream failed");
   }
}

void TcpClientPort::Impl::initUnix(const std::string &path)
{
   struct sockaddr_un sockaddr;
   socklen_t sockaddrLen;

   tsd::communication::messaging::parseUnixPath(path, sockaddr, sockaddrLen);

   signal(SIGPIPE, SIG_IGN);

   bool ok = m_select.init();
   if (!ok) {
      throw ConnectionException("Selector init failed!");
   }

   int sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
   if (sock < 0) {
      int err = errno;
      throw ConnectionException(std::string("Could not create socket: ") +
         std::strerror(err));
   }

   int ret;
   do {
      ret = connect(sock, (struct sockaddr *) &sockaddr, sockaddrLen);
   } while (ret < 0 && errno == EINTR);
   if (ret < 0) {
      int err = errno;
      close(sock);
      throw ConnectionException(std::string("connect failed: ") +
         std::strerror(err));
   }

   if (!TcpEndpoint::init(sock, m_select)) {
      throw ConnectionException("TcpEndpoint init failed");
   }

   if (!initUpstream(m_subDomain)) {
      throw ConnectionException("initUpstream failed");
   }
}

bool TcpClientPort::Impl::sendPacket(std::auto_ptr<Packet> pkt)
{
   return epSendPacket(pkt);
}

void TcpClientPort::Impl::epReceivedPacket(std::auto_ptr<Packet> pkt)
{
   receivedPacket(pkt);
}

void TcpClientPort::Impl::epDisconnected()
{
   tsd::common::system::MutexGuard g(m_lock);
   m_log << tsd::common::logging::LogLevel::Warn
         << "Peer disconnected!" << &std::endl;

   // regardless of the context we hand this over to the io-thread
   m_running = false;
   g.unlock();

   m_select.interrupt();
}

bool TcpClientPort::Impl::startPort()
{
   m_running = true;
   start();
   return true;
}

void TcpClientPort::Impl::stopPort()
{
   tsd::common::system::MutexGuard g(m_lock);
   m_running = false;
   g.unlock();
   m_select.interrupt();
   join();
}

void TcpClientPort::Impl::run()
{
   connected();

   tsd::common::system::MutexGuard g(m_lock);

   while (m_running) {
      // dispatch and wait for interrupt
      g.unlock();
      m_select.dispatch();
      g.lock();
   }

   g.unlock();

   disconnected();
}

/*****************************************************************************/

TcpClientPort::TcpClientPort(tsd::communication::messaging::Router &router,
                             const std::string &subDomain)
   : m_p(new Impl(router, subDomain))
{
}

TcpClientPort::~TcpClientPort()
{
   m_p->disconnect();
   delete m_p;
}

void TcpClientPort::disconnect()
{
   m_p->disconnect();
}

void TcpClientPort::initV4(uint32_t addr, uint16_t port, uint32_t sndBuf, uint32_t rcvBuf)
{
   m_p->initV4(addr, port, sndBuf, rcvBuf);
}

void TcpClientPort::initUnix(const std::string &path)
{
   m_p->initUnix(path);
}
