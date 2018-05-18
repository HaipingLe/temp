
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <tsd/common/system/Thread.hpp>
#include <tsd/common/utils/DuplexShm.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>

#include "ConnectionImpl.hpp"
#include "UioShmPort.hpp"


namespace tsd { namespace communication { namespace messaging {

   class Router;

   class UioShmPortHalf
      : public tsd::common::utils::DuplexShm
      , public IConnectionCallbacks
   {
      int m_uioFd;
      int m_wakeFd;
      void *m_shmPtr;
      size_t m_shmLen;
      uint8_t m_bounceBuffer[4096];

      void init(const std::string &url);

   protected:
      ConnectionImpl *m_connection;

      UioShmPortHalf(Router &router);
      ~UioShmPortHalf();

      // tsd::common::utils::DuplexShm
      void wakeRemote();
      bool waitForRemote(uint32_t timeout);
      void processData(const void* data, uint32_t size);
      bool getData(const void*& data, uint32_t& size);
      void consumeData(const uint32_t amount);

      // IConnectionCallbacks
      void wakeup();

   public:
      void initConnect(const std::string &url, const std::string &subDomain);
      void initListen(const std::string &url);
      void disconnect();
   };

   class UioShmPort::Listener
      : public UioShmPortHalf
      , protected tsd::common::system::Thread
   {
   protected:
      void run(); // tsd::common::system::Thread
      bool setupConnection(); // IConnectionCallbacks
      void teardownConnection(); // IConnectionCallbacks

   public:
      Listener(Router &router);
      ~Listener();
   };

   class UioShmPort::Connector
      : public UioShmPortHalf
      , protected tsd::common::system::Thread
   {
   protected:
      void run(); // tsd::common::system::Thread
      bool setupConnection(); // IConnectionCallbacks
      void teardownConnection(); // IConnectionCallbacks

   public:
      Connector(Router &router);
      ~Connector();
   };

} } }

using tsd::communication::messaging::UioShmPort;
using tsd::communication::messaging::UioShmPortHalf;

/*****************************************************************************/

UioShmPortHalf::UioShmPortHalf(Router &router)
   : m_uioFd(-1)
   , m_wakeFd(-1)
   , m_shmPtr(MAP_FAILED)
   , m_shmLen(0)
   , m_connection(0)
{
   m_connection = new ConnectionImpl(*this, router);
}

UioShmPortHalf::~UioShmPortHalf()
{
   m_connection->disconnect();
   delete m_connection;

   if (m_shmPtr != MAP_FAILED) {
      munmap(m_shmPtr, m_shmLen);
   }

   if (m_uioFd != -1) {
      close(m_uioFd);
   }

   if (m_wakeFd != -1) {
      close(m_wakeFd);
   }
}

void UioShmPortHalf::init(const std::string &url)
{
   m_wakeFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
   if (m_wakeFd == -1) {
      int err = errno;
      throw ConnectionException(std::string("Cannot create eventfd: ") +
         std::strerror(err));
   }

   struct stat sb;
   std::memset(&sb, 0, sizeof(sb));
   if (stat(url.c_str(), &sb) == -1) {
      int err = errno;
      throw ConnectionException(std::string("Cannot stat ") + url + ": " +
         std::strerror(err));
   }

   std::stringstream sysfs;
   sysfs << "/sys/dev/char/" << major(sb.st_rdev) << ":" << minor(sb.st_rdev)
      << "/maps/map0/size";
   std::ifstream sizeFile(sysfs.str().c_str());
   if (!sizeFile.is_open()) {
      throw ConnectionException("Cannot open " + sysfs.str());
   }
   sizeFile >> std::hex >> m_shmLen;

   m_uioFd = open(url.c_str(), O_RDWR|O_SYNC);
   if (m_uioFd == -1) {
      int err = errno;
      throw ConnectionException(std::string("Cannot open ") + url + ": " +
         std::strerror(err));
   }

   m_shmPtr = mmap(NULL, m_shmLen, PROT_READ | PROT_WRITE, MAP_SHARED, m_uioFd, 0);
   if (m_shmPtr == MAP_FAILED) {
      int err = errno;
      throw ConnectionException(std::string("mmap failed: ") +
         std::strerror(err));
   }

   setAlignBuffer(m_bounceBuffer, sizeof(m_bounceBuffer));
}

void UioShmPortHalf::initListen(const std::string &url)
{
   init(url);
   if (!tsd::common::utils::DuplexShm::initListen(m_shmPtr, static_cast<uint32_t>(m_shmLen))) {
      throw ConnectionException("initListen failed");
   }

   if (!m_connection->listenDownstream()) {
      throw ConnectionException("listenDownstream failed");
   }
}

void UioShmPortHalf::initConnect(const std::string &url, const std::string &subDomain)
{
   init(url);
   if (!tsd::common::utils::DuplexShm::initConnect(m_shmPtr,  static_cast<uint32_t>(m_shmLen))) {
      throw ConnectionException("initConnect failed");
   }

   if (!m_connection->connectUpstream(subDomain)) {
      throw ConnectionException("connectUpstream failed");
   }
}

// tsd::common::utils::DuplexShm
void UioShmPortHalf::wakeRemote()
{
   uint32_t val = 1;
   ssize_t ret;

   do {
      ret = write(m_uioFd, &val, sizeof(val));
   } while (ret == -1 && errno == EINTR);

   // no furhter error handling as we cannot do anything really here
}

// tsd::common::utils::DuplexShm
bool UioShmPortHalf::waitForRemote(uint32_t timeout)
{
   int max = std::max(m_wakeFd, m_uioFd);

   fd_set rset;
   FD_ZERO(&rset);
   FD_SET(m_wakeFd, &rset);
   FD_SET(m_uioFd, &rset);

   // Build the timeout
   struct timeval tv;
   tv.tv_sec  = timeout / 1000;
   tv.tv_usec = (timeout % 1000) * 1000UL;

   // Linux will update struct timeval if the sleep was interrupted.
   int ret;
   do {
      ret = select(max+1, &rset, 0, 0, &tv);
      if (ret > 0) {
         if (FD_ISSET(m_wakeFd, &rset)) {
            eventfd_t unused;
            eventfd_read(m_wakeFd, &unused);
         }
         if (FD_ISSET(m_uioFd, &rset)) {
            int32_t unused;
            read(m_uioFd, &unused, sizeof(unused));
         }
      }
   } while (ret < 0 && ret == EINTR);

   return ret > 0;
}

// tsd::common::utils::DuplexShm
void UioShmPortHalf::processData(const void* data, uint32_t size)
{
   m_connection->processData(data, size);
}

// tsd::common::utils::DuplexShm
bool UioShmPortHalf::getData(const void*& data, uint32_t& size)
{
   size_t s;
   bool ret = m_connection->getData(data, s);
   size = static_cast<uint32_t>(s);
   return ret;
}

// tsd::common::utils::DuplexShm
void UioShmPortHalf::consumeData(const uint32_t amount)
{
   m_connection->consumeData(amount);
}

// IConnectionCallbacks
void UioShmPortHalf::wakeup()
{
   eventfd_write(m_wakeFd, 1);
}

void UioShmPortHalf::disconnect()
{
   m_connection->disconnect();
}

/*****************************************************************************/

UioShmPort::Listener::Listener(Router &router)
   : UioShmPortHalf(router)
   , tsd::common::system::Thread("tsd.communication.messaging.UioShmPort.Listener")
{ }

UioShmPort::Listener::~Listener()
{ }

void UioShmPort::Listener::run()
{
   bool ok;

   do {
      ok = waitForConnect();
      if (ok) {
         m_connection->setConnected();
         ok = operate();
         m_connection->setDisconnected();
      }
   } while (ok);
}

bool UioShmPort::Listener::setupConnection()
{
   start();
   return true;
}

void UioShmPort::Listener::teardownConnection()
{
   finish();
   wakeup();
   join();
}

/*****************************************************************************/

UioShmPort::Connector::Connector(Router &router)
   : UioShmPortHalf(router)
   , tsd::common::system::Thread("tsd.communication.messaging.UioShmPort.Connector")
{ }

UioShmPort::Connector::~Connector()
{ }

void UioShmPort::Connector::run()
{
   bool ok;

   ok = connectRemote();
   if (ok) {
      m_connection->setConnected();
      ok = operate();
      m_connection->setDisconnected();
   }
}

bool UioShmPort::Connector::setupConnection()
{
   start();
   return true;
}

void UioShmPort::Connector::teardownConnection()
{
   finish();
   wakeup();
   join();
}

/*****************************************************************************/

UioShmPort::UioShmPort(Router &router)
   : m_router(router)
   , m_listener(0)
   , m_connector(0)
{ }

UioShmPort::~UioShmPort()
{
   UioShmPort::disconnect();
}

void UioShmPort::connectUpstream(const std::string &url, const std::string &subDomain)
{
   if (m_connector || m_listener) {
      throw ConnectionException("Already connected");
   }

   std::auto_ptr<Connector> connector(new Connector(m_router));
   connector->initConnect(url, subDomain);
   m_connector = connector.release();
}

void UioShmPort::listenDownstream(const std::string &url)
{
   if (m_connector || m_listener) {
      throw ConnectionException("Already connected");
   }

   std::auto_ptr<Listener> listener(new Listener(m_router));
   listener->initListen(url);
   m_listener = listener.release();
}

void UioShmPort::disconnect()
{
   if (m_listener != 0) {
      m_listener->disconnect();
      delete m_listener;
      m_listener = 0;
   }

   if (m_connector != 0) {
      m_connector->disconnect();
      delete m_connector;
      m_connector = 0;
   }
}
