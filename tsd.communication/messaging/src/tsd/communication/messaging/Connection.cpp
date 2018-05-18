
#include <cstdlib>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/communication/messaging/Connection.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>

#include "ConnectionImpl.hpp"
#include "Router.hpp"

#ifdef TARGET_OS_POSIX_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "TcpClientPort.hpp"
#include "TcpServerPort.hpp"
#include "UioShmPort.hpp"

namespace {

   bool parseV4(const std::string &name, uint32_t &addr, uint16_t &port)
   {
      bool ret = true;

      if (name.length() > 0) {
         std::string::size_type colon = name.find(":");
         if (colon != std::string::npos) {
            port = static_cast<uint16_t>(std::atol(name.substr(colon+1).c_str()));
         }

         std::string host(name.substr(0, colon));
         struct in_addr tmp;
         ret = inet_aton(host.c_str(), &tmp) == 1;
         if (ret) {
            addr = ntohl(tmp.s_addr);
         }
      }

      return ret;
   }

   const std::string DEFAULT_UNIX_PATH("@tsd.communication.commgr");

}
#endif

using tsd::communication::messaging::Connection;
using tsd::communication::messaging::ConnectionException;
using tsd::communication::messaging::IConnection;
using tsd::communication::messaging::IConnectionCallbacks;


IConnection::~IConnection()
{
}


IConnection*
tsd::communication::messaging::connectUpstream(const std::string &url,
                                               const std::string &subDomain)
{
   IConnection *connection = NULL;

#ifdef TARGET_OS_POSIX_LINUX
   if (url.empty()) {
      std::auto_ptr<TcpClientPort> p(new TcpClientPort(Router::getLocalRouter(), subDomain));
#ifdef TARGET_TYPE_EMBEDDED
      p->initUnix(DEFAULT_UNIX_PATH);
#else
      p->initV4();
#endif
      connection = p.release();
   } else if (url.compare(0, 6, "tcp://") == 0) {
      std::auto_ptr<TcpClientPort> p(new TcpClientPort(Router::getLocalRouter(), subDomain));
      uint32_t addr = INADDR_LOOPBACK;
      uint16_t port = 24710;
      if (parseV4(url.substr(6), addr, port)) {
         p->initV4(addr, port);
         connection = p.release();
      } else {
         throw ConnectionException("Could not parse: " + url);
      }
   } else if (url.compare(0, 7, "unix://") == 0) {
      std::auto_ptr<TcpClientPort> p(new TcpClientPort(Router::getLocalRouter(), subDomain));
      std::string path(url.substr(7));
      p->initUnix(path.empty() ? DEFAULT_UNIX_PATH : path);
      connection = p.release();
   } else if (url.compare(0, 6, "uio://") == 0) {
      std::auto_ptr<UioShmPort> p(new UioShmPort(Router::getLocalRouter()));
      p->connectUpstream(url.substr(6), subDomain);
      connection = p.release();
   }
#else
   (void)subDomain;
#endif

   if (connection == NULL) {
      throw ConnectionException("Invalid address: " + url);
   }

   tsd::common::logging::Logger log("tsd.communication.messaging");
   log << tsd::common::logging::LogLevel::Info
       << "Connected to [" << url << "] as [" << subDomain << "]"
       << &std::endl;

   return connection;
}

IConnection*
tsd::communication::messaging::listenDownstream(const std::string &url)
{
   IConnection *connection = NULL;

#ifdef TARGET_OS_POSIX_LINUX
   if (url.compare(0, 6, "tcp://") == 0) {
      std::auto_ptr<TcpServerPort> p(new TcpServerPort(Router::getLocalRouter()));
      uint32_t addr = INADDR_ANY;
      uint16_t port = 24710;
      if (parseV4(url.substr(6), addr, port)) {
         p->initV4(addr, port);
         connection = p.release();
      } else {
         throw ConnectionException("Could not parse: " + url);
      }
   } else if (url.compare(0, 7, "unix://") == 0) {
      std::auto_ptr<TcpServerPort> p(new TcpServerPort(Router::getLocalRouter()));
      std::string path(url.substr(7));
      p->initUnix(path.empty() ? DEFAULT_UNIX_PATH : path);
      connection = p.release();
   } else if (url.compare(0, 6, "uio://") == 0) {
      std::auto_ptr<UioShmPort> p(new UioShmPort(Router::getLocalRouter()));
      p->listenDownstream(url.substr(6));
      connection = p.release();
   }
#endif

   if (connection == NULL) {
      throw ConnectionException("Invalid address: " + url);
   }

   return connection;
}

/*****************************************************************************/

IConnectionCallbacks::~IConnectionCallbacks()
{
}

/*****************************************************************************/

Connection::Connection()
   : m_p(0)
{
   m_p = new ConnectionImpl(*this, Router::getLocalRouter());
}

Connection::~Connection()
{
   m_p->disconnect();
   delete m_p;
}

void Connection::setConnected()
{
   m_p->setConnected();
}

void Connection::setDisconnected()
{
   m_p->setDisconnected();
}

void Connection::processData(const void* data, size_t size)
{
   m_p->processData(data, size);
}

bool Connection::getData(const void*& data, size_t& size)
{
   return m_p->getData(data, size);
}

void Connection::consumeData(size_t amount)
{
   m_p->consumeData(amount);
}

bool Connection::connectUpstream(const std::string &subDomain)
{
   return m_p->connectUpstream(subDomain);
}

bool Connection::listenDownstream()
{
   return m_p->listenDownstream();
}

void Connection::disconnect()
{
   m_p->disconnect();
}
