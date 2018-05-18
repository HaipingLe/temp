
#include <cassert>
#include <cstring>

#include <tsd/common/ipc/networkinteger.h>
#include <tsd/common/logging/Logger.hpp>

#include "IPort.hpp"
#include "Packet.hpp"
#include "Router.hpp"

using tsd::common::ipc::NetworkInteger;
using tsd::communication::event::IfcAddr_t;
using tsd::communication::messaging::IPort;
using tsd::communication::messaging::Packet;

namespace {

   struct DhcpOffer {
      NetworkInteger<uint32_t>   m_version;
      uint8_t                    m_prefixLength;
      uint8_t                    m_padding[3];
      NetworkInteger<uint64_t>   m_addr;
      NetworkInteger<uint64_t>   m_nameServer;
   };

}

IPort::IPort(Router &router)
   : m_router(router)
   , m_localAddr(0)
   , m_peerAddr(0)
   , m_mask(INVALID_ADDR_MASK)
   , m_prefixLength(0)
   , m_state(UNINITIALIZED)
   , m_upstream(false)
   , m_nameServer(0)
   , m_ioThreadId(0)
{
}

IPort::~IPort()
{
}

bool IPort::initDownstream()
{
   bool ret = false;

   if (m_state == UNINITIALIZED) {
      m_state = DOWNSTREAM_UNCONNECTED;
      m_upstream = false;
      ret = startPort();
      if (!ret) {
         m_state = UNINITIALIZED;
      }
   }

   return ret;
}

bool IPort::initUpstream(const std::string &subDomain)
{
   tsd::common::logging::Logger log("tsd.communication.messaging");
   bool ret = false;

   if (m_state == UNINITIALIZED) {
      if (m_router.hasUpstreamPort()) {
         log << tsd::common::logging::LogLevel::Error
             << "IPort::initUpstream on [" << m_router.getName() << "]: router already connected"
             << &std::endl;
         return false;
      }

      m_state = UPSTREAM_UNBOUND;
      m_upstream = true;

      ret = startPort();
      if (ret) {
         tsd::common::system::MutexGuard g(m_lock);
         while (m_state == UPSTREAM_UNBOUND) {
            m_stateCondition.wait(m_lock);
         }

         switch (m_state) {
            case UPSTREAM_BOUND:
               g.unlock();
               ret = m_router.addUpstreamPort(this, m_nameServer, subDomain);
               g.lock();

               // We did drop the lock. Check the state again as we may not be
               // connected anymore...
               if (ret && m_state == UPSTREAM_BOUND) {
                  m_state = UPSTREAM_CONNECTED;
               } else if (ret) {
                  log << tsd::common::logging::LogLevel::Error
                      << "IPort::initUpstream on [" << m_router.getName()
                      << "]: disconnected while adding. state: " << m_state
                      << &std::endl;
                  g.unlock();
                  m_router.delPort(this);
                  g.lock();
                  ret = false;
               } else {
                  log << tsd::common::logging::LogLevel::Error
                      << "IPort::initUpstream on [" << m_router.getName() << "]: router rejected port"
                      << &std::endl;
               }
               break;
            case UPSTREAM_DEAD:
               log << tsd::common::logging::LogLevel::Error
                   << "IPort::initUpstream on [" << m_router.getName() << "]: connection interrupted"
                   << &std::endl;
               ret = false;
               break;
            default:
               log << tsd::common::logging::LogLevel::Error
                   << "IPort::initUpstream on [" << m_router.getName() << "]: WTF: " << m_state
                   << &std::endl;
               ret = false;
               break;
         }

         if (ret) {
            log << tsd::common::logging::LogLevel::Debug
                << "IPort::initUpstream on [" << m_router.getName()
                << "]: connected as [" << subDomain << "]" << &std::endl;
         } else {
            g.unlock();
            stopPort();
            g.lock();
            m_state = UNINITIALIZED;
         }
      } else {
         m_state = UNINITIALIZED;
         log << tsd::common::logging::LogLevel::Error
             << "IPort::initUpstream on [" << m_router.getName() << "]: startPort failed"
             << &std::endl;
      }
   } else {
      log << tsd::common::logging::LogLevel::Error
          << "IPort::initUpstream on [" << m_router.getName() << "]: already initialized"
          << &std::endl;
   }

   return ret;
}

void IPort::finish()
{
   tsd::common::system::MutexGuard g(m_lock);

   tsd::common::logging::Logger log("tsd.communication.messaging");
   log << tsd::common::logging::LogLevel::Info
       << "IPort::finish on [" << m_router.getName() << "]: state: " << m_state
       << &std::endl;

   /*
    * First stop port if it is still running. stopPort() is expected to block
    * until the io-thread of the port is finished. This implies that no
    * connected()/disconnected() calls are expected anymore.
    */
   switch (m_state) {
      case DOWNSTREAM_CONNECTED:
      case DOWNSTREAM_UNCONNECTED:
      case UPSTREAM_CONNECTED:
      case UPSTREAM_DEAD:
         g.unlock();
         stopPort();
         g.lock();
         break;

      default:
         // not running
         break;
   }

   // clean up state that was left behind
   switch (m_state) {
      case UNINITIALIZED:
         // do nothing
         break;

      case UPSTREAM_BOUND:
      case UPSTREAM_UNBOUND:
         // This is certainly broken. Another thread is currently in
         // initUpstream(). Still do the right thing and hope for the best.
         m_state = UPSTREAM_DEAD;
         break;

      case DOWNSTREAM_CONNECTED:
      case UPSTREAM_CONNECTED:
      {
         State oldState = m_state;
         IfcAddr_t oldAddr = m_localAddr;

         g.unlock();
         m_router.delPort(this);
         if (oldState == DOWNSTREAM_CONNECTED) {
            m_router.freeDownstreamAddr(oldAddr);
         }
         g.lock();

         // Clear state and address *after* unregistering. Router::delPort()
         // will use the port address for various maintenance operations. Make
         // sure the address is valid as long as the port is known to the
         // router.
         m_state = UNINITIALIZED;
         m_localAddr = 0;
         m_peerAddr = 0;
         m_mask = INVALID_ADDR_MASK;
         m_prefixLength = 0;
         break;
      }
      case DOWNSTREAM_UNCONNECTED:
      case UPSTREAM_DEAD:
         m_state = UNINITIALIZED;
         m_localAddr = 0;
         m_peerAddr = 0;
         m_mask = INVALID_ADDR_MASK;
         m_prefixLength = 0;
         break;
   }
}

bool IPort::connected()
{
   bool ret = false;
   IfcAddr_t addr, nameServer;
   uint8_t prefixLength;

   tsd::common::system::MutexGuard g(m_lock);

   assert(m_ioThreadId == 0);
   m_ioThreadId = tsd::common::system::Thread::myself();

   switch (m_state) {
      case DOWNSTREAM_UNCONNECTED:
         g.unlock();
         ret = m_router.allocateDownstreamAddr(addr, prefixLength, nameServer);
         g.lock();
         if (ret) {
            m_localAddr = addr;
            m_peerAddr = addr + (1u << INTERFACE_ADDR_SIZE);
            m_mask = HOST_ADDR_MASK << (HOST_ADDR_SIZE - prefixLength);
            m_prefixLength = prefixLength;
            m_state = DOWNSTREAM_CONNECTED;

            DhcpOffer offer;
            std::memset(&offer, 0, sizeof(offer));
            offer.m_version = 1;
            offer.m_prefixLength = prefixLength;
            offer.m_addr = addr;
            offer.m_nameServer = nameServer;
            std::auto_ptr<Packet> pkt(new Packet(Packet::DHCP_OFFER, 0, 0, 0, (const
                  char*)&offer, sizeof(offer)));

            g.unlock();
            sendPacket(pkt);
            m_router.addDownstreamPort(this);
         }
         break;

      case UPSTREAM_UNBOUND:
         ret = true;
         break;

      default:
         // protocol violation
         break;
   }

   return ret;
}

void IPort::disconnected()
{
   tsd::common::logging::Logger log("tsd.communication.messaging");
   tsd::common::system::MutexGuard g(m_lock);

   assert(m_ioThreadId == tsd::common::system::Thread::myself());
   m_ioThreadId = 0;

   switch (m_state) {
      case UPSTREAM_UNBOUND:
      case UPSTREAM_BOUND:
         m_state = UPSTREAM_DEAD;
         m_stateCondition.signal();
         break;
      case UPSTREAM_CONNECTED:
         m_state = UPSTREAM_DEAD;
         m_stateCondition.signal();
         log << tsd::common::logging::LogLevel::Error
             << "IPort::disconnected upstream on [" << m_router.getName() << "]"
             << &std::endl;
         g.unlock();
         m_router.delPort(this);
         break;
      case DOWNSTREAM_CONNECTED:
         m_state = DOWNSTREAM_UNCONNECTED;
         log << tsd::common::logging::LogLevel::Warn
             << "IPort::disconnected downstream on [" << m_router.getName() << "]"
             << &std::endl;
         g.unlock();
         m_router.delPort(this);
         m_router.freeDownstreamAddr(m_localAddr);
         break;

      case UNINITIALIZED:
      case UPSTREAM_DEAD:
      case DOWNSTREAM_UNCONNECTED:
         // do nothing
         break;
   }
}

void IPort::receivedPacket(std::auto_ptr<Packet> pkt)
{
   // TODO: get rid of this lock for the usual case (*_CONNECTED)
   tsd::common::system::MutexGuard g(m_lock);

   assert(m_ioThreadId == tsd::common::system::Thread::myself());

   switch (m_state) {
      case UPSTREAM_BOUND:
      case UPSTREAM_CONNECTED:
      case DOWNSTREAM_CONNECTED:
         g.unlock();
         m_router.routePacket(pkt, this);
         break;
      case UPSTREAM_UNBOUND:
         processUnboundPacket(pkt);
         break;
      default:
         // ignore packet
         break;
   }
}

void IPort::processUnboundPacket(std::auto_ptr<Packet> pkt)
{
   if (pkt->getType() != Packet::DHCP_OFFER) {
      m_state = UPSTREAM_DEAD;
      m_stateCondition.signal();
      return;
   }

   DhcpOffer offer;
   if (pkt->getBufferLength() != sizeof(offer)) {
      m_state = UPSTREAM_DEAD;
      m_stateCondition.signal();
      return;
   }
   std::memcpy(&offer, pkt->getBufferPtr(), sizeof(offer));

   if (offer.m_version != 1) {
      m_state = UPSTREAM_DEAD;
      m_stateCondition.signal();
      return;
   }

   m_localAddr = offer.m_addr + (1u << INTERFACE_ADDR_SIZE);
   m_peerAddr = offer.m_addr;
   m_mask = HOST_ADDR_MASK << 1;
   m_prefixLength = offer.m_prefixLength;
   m_nameServer = offer.m_nameServer;
   m_state = UPSTREAM_BOUND;

   m_stateCondition.signal();

   return;
}
