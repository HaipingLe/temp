
#include <iomanip>
#include <deque>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/common/system/Thread.hpp>

#include <tsd/communication/messaging/Packet.hpp>
#include <tsd/communication/messaging/Router.hpp>

#include "InternalPort.hpp"

using tsd::communication::event::IfcAddr_t;

namespace tsd { namespace communication { namespace messaging {

class InternalPortSouth;

/**
 * Downstream port of north router.
 */
class InternalPortNorth
   : protected tsd::common::system::Thread
   , public IPort
{
   typedef std::deque<Packet*> PacketQueue;

   InternalPortSouth *m_other;
   tsd::common::system::Mutex m_lock;
   tsd::common::system::CondVar m_queueCondition;
   bool m_running;
   bool m_connected;
   bool m_idle;
   PacketQueue m_queue;

protected:
   void run(); // tsd::common::system::Thread
   bool startPort(); // IPort
   void stopPort(); // IPort

public:
   InternalPortNorth(Router &router);
   ~InternalPortNorth();

   bool init(InternalPortSouth *other);
   bool queuePacket(std::auto_ptr<Packet> pkt);
   bool isIdle();

   bool sendPacket(std::auto_ptr<Packet> pkt); // IPort
};

/**
 * Upstream port of south router.
 */
class InternalPortSouth
   : protected tsd::common::system::Thread
   , public IPort
{
   typedef std::deque<Packet*> PacketQueue;

   InternalPortNorth *m_other;
   tsd::common::system::Mutex m_lock;
   tsd::common::system::CondVar m_queueCondition;
   bool m_running;
   bool m_idle;
   PacketQueue m_queue;

protected:
   void run(); // tsd::common::system::Thread
   bool startPort();   // IPort
   void stopPort();   // IPort

public:
   InternalPortSouth(Router &router);
   ~InternalPortSouth();

   bool init(InternalPortNorth *other, const std::string &subDomain);
   bool queuePacket(std::auto_ptr<Packet> pkt);
   bool isIdle();

   bool sendPacket(std::auto_ptr<Packet> pkt); // IPort
};

/*****************************************************************************/

InternalPortNorth::InternalPortNorth(Router &router)
   : tsd::common::system::Thread("InternalPortNorth")
   , IPort(router)
   , m_other(NULL)
   , m_running(false)
   , m_connected(false)
   , m_idle(true)
{ }

InternalPortNorth::~InternalPortNorth()
{
}

void InternalPortNorth::run()
{
   tsd::common::system::MutexGuard g(m_lock);

   do {
      if (!m_connected && m_other != NULL) {
         m_connected = true;
         g.unlock();
         connected();
         g.lock();
      } else if (m_connected && m_other == NULL) {
         m_connected = false;
         g.unlock();
         disconnected();
         g.lock();
      }

      while (!m_queue.empty()) {
            std::auto_ptr<Packet> p(m_queue.front());
            m_queue.pop_front();
            g.unlock();
            receivedPacket(p);
            g.lock();
      }
      m_idle = true;

      // check before sleeping!
      if (!m_running) {
         break;
      }
      m_queueCondition.wait(m_lock);
   } while (m_running);

   if (m_connected) {
      g.unlock();
      disconnected();
   }
}

bool InternalPortNorth::startPort()
{
   m_idle = true;
   m_running = true;
   start();
   return true;
}

void InternalPortNorth::stopPort()
{
   tsd::common::system::MutexGuard g(m_lock);
   m_running = false;
   m_queueCondition.signal();
   g.unlock();
   join();

   while (!m_queue.empty()) {
      delete m_queue.front();
      m_queue.pop_front();
   }
}

bool InternalPortNorth::init(InternalPortSouth *other)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_other = other;
   m_queueCondition.signal();
   g.unlock();

   return initDownstream();
}

bool InternalPortNorth::queuePacket(std::auto_ptr<Packet> pkt)
{
   bool ret = false;

   tsd::common::system::MutexGuard g(m_lock);
   if (m_connected) {
      m_idle = false;
      m_queue.push_back(pkt.release());
      m_queueCondition.signal();
      ret = true;
   }

   return ret;
}

bool InternalPortNorth::sendPacket(std::auto_ptr<Packet> pkt)
{
   bool ret = false;
   tsd::common::system::MutexGuard g(m_lock);
   if (m_connected) {
      InternalPortSouth *other = m_other;
      g.unlock();
      ret = other->queuePacket(pkt);
   }
   return ret;
}

bool InternalPortNorth::isIdle()
{
   tsd::common::system::MutexGuard g(m_lock);
   return m_idle;
}

/*****************************************************************************/

InternalPortSouth::InternalPortSouth(Router &router)
   : tsd::common::system::Thread("InternalPortSouth")
   , IPort(router)
   , m_other(NULL)
   , m_running(false)
   , m_idle(true)
{ }

InternalPortSouth::~InternalPortSouth()
{
}

void InternalPortSouth::run()
{
   connected();

   tsd::common::system::MutexGuard g(m_lock);

   do {
      while (!m_queue.empty()) {
            std::auto_ptr<Packet> p(m_queue.front());
            m_queue.pop_front();
            g.unlock();
            receivedPacket(p);
            g.lock();
      }
      m_idle = true;

      // check before sleeping!
      if (!m_running) {
         break;
      }
      m_queueCondition.wait(m_lock);
   } while (m_running);

   g.unlock();
   disconnected();
}

bool InternalPortSouth::startPort()
{
   m_running = true;
   start();
   return true;
}

void InternalPortSouth::stopPort()
{
   tsd::common::system::MutexGuard g(m_lock);
   m_running = false;
   m_queueCondition.signal();
   g.unlock();
   join();

   while (!m_queue.empty()) {
      delete m_queue.front();
      m_queue.pop_front();
   }
}

bool InternalPortSouth::init(InternalPortNorth *other, const std::string &subDomain)
{
   m_other = other;
   return initUpstream(subDomain);
}

bool InternalPortSouth::queuePacket(std::auto_ptr<Packet> pkt)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_idle = false;
   m_queue.push_back(pkt.release());
   m_queueCondition.signal();
   return true;
}

bool InternalPortSouth::sendPacket(std::auto_ptr<Packet> pkt)
{
   return m_other->queuePacket(pkt);
}

bool InternalPortSouth::isIdle()
{
   tsd::common::system::MutexGuard g(m_lock);
   return m_idle;
}

/*****************************************************************************/

class InternalPortBridge
{
   InternalPortNorth m_north;
   InternalPortSouth m_south;

public:
   InternalPortBridge(Router &upstreamRouter, Router &downstreamRouter)
      : m_north(upstreamRouter)
      , m_south(downstreamRouter)
   {
   }

   ~InternalPortBridge()
   {
      m_north.finish();
      m_south.finish();
   }

   bool init(const std::string &subDomain)
   {
      if (!m_north.init(&m_south)) {
         return false;
      }

      if (!m_south.init(&m_north, subDomain)) {
         m_north.finish();
         return false;
      }

      tsd::common::logging::Logger log("tsd.communication.messaging.internalport");
      log << tsd::common::logging::LogLevel::Info
          << "InternalPort::connect: addr:"
          << std::hex << std::setfill('0')
          << std::setw(16) << m_north.getLocalAddr() << "/"
          << std::dec << (uint32_t)m_north.getPrefixLength()
          << ", " << m_north.getRouter().getName()
          << " <- " << m_south.getRouter().getName()
          << std::endl;

      return true;
   }

   bool isIdle()
   {
      return m_north.isIdle() && m_south.isIdle();
   }
};


/*****************************************************************************/

InternalPort::InternalPort()
   : m_bridge(NULL)
{
}

InternalPort::~InternalPort()
{
   if (m_bridge) {
      delete m_bridge;
   }
}

bool InternalPort::connect(Router &upstreamRouter,
                           Router &downstreamRouter,
                           const std::string &subDomain)
{
   // already connected?
   if (m_bridge) {
      return false;
   }

   InternalPortBridge *bridge = new InternalPortBridge(upstreamRouter, downstreamRouter);
   if (!bridge->init(subDomain)) {
      delete bridge;
      return false;
   }

   m_bridge = bridge;
   return true;
}

bool InternalPort::isIdle()
{
   bool idle = true;

   if (m_bridge) {
      idle = m_bridge->isIdle();
   }

   return idle;
}

} } }
