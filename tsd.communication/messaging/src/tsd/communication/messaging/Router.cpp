
#include <iomanip>

#include <tsd/common/ipc/rpcbuffer.h>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/communication/messaging/BaseException.hpp>
#include <tsd/communication/messaging/InvalidArgumentException.hpp>

#include "IPort.hpp"
#include "NameServer.hpp"
#include "QueueInternal.hpp"
#include "Router.hpp"
#include "Packet.hpp"

using tsd::communication::event::TsdEvent;
using tsd::communication::event::IfcAddr_t;
using tsd::communication::event::LOOPBACK_ADDRESS;

namespace tsd { namespace communication { namespace messaging {

Router::Router(const std::string &name)
   : m_name(name)
   , m_log("tsd.communication.messaging.router")
   , m_ifcSeqNum(1) // TODO: random?
   , m_defaultGateway(NULL)
   , m_subNetPrefixLength(8)
   , m_subNetSeqNum(1) // TODO: random?
{
   m_nameServer.reset(new NameServer(*this));
   m_nameServer->init();
}

Router::~Router()
{
   m_nameServer->fini();
}

IfcAddr_t Router::allocateIfcAddrInternal()
{
   uint32_t newAddr;

   do {
      newAddr = ++m_ifcSeqNum;
   } while (m_ifcAddrs.find(newAddr) != m_ifcAddrs.end());

   return newAddr;
}

void Router::freeIfcAddrInternal(IfcAddr_t address)
{
   m_ifcAddrs.erase(static_cast<IfcAddrs::key_type>(address));
}

IfcAddr_t Router::allocateIfcAddr(Queue *queue)
{
   tsd::common::system::MutexGuard g(m_lock);

   IfcAddr_t newAddr = allocateIfcAddrInternal();
   m_addr2Queue[newAddr] = queue;
   return newAddr;
}

void Router::freeIfcAddr(IfcAddr_t address)
{
   tsd::common::system::MutexGuard g(m_lock);

   m_addr2Queue.erase(address);
   freeIfcAddrInternal(address);
   sendDeathMessage(address, address);
}

/**
 * Configure prefix length for address allocation.
 *
 * The reconfiguration of the prefix length is only allowed if no subnets are
 * currently allocated.
 */
bool Router::setSubnetConfig(uint8_t prefixLength)
{
   tsd::common::system::MutexGuard g(m_lock);

   bool ret = false;
   if (m_subNets.empty()) {
      m_subNetPrefixLength = prefixLength;
      ret = true;
   }

   return ret;
}

bool Router::allocateDownstreamAddr(tsd::communication::event::IfcAddr_t &addr,
                                    uint8_t &prefix,
                                    tsd::communication::event::IfcAddr_t &nameServer)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_subNetPrefixLength == 0) {
      return false;
   }

   uint8_t ourPrefixLength = (m_defaultGateway != NULL)
      ? m_defaultGateway->getPrefixLength()
      : 0;
   tsd::communication::event::IfcAddr_t ourNetAddr = (m_defaultGateway != NULL)
      ? (m_defaultGateway->getLocalAddr() & m_defaultGateway->getMask())
      : 0;

   uint8_t newPrefixLength = static_cast<uint8_t>(ourPrefixLength + m_subNetPrefixLength);
   uint8_t newPrefixShift = static_cast<uint8_t>(64u - newPrefixLength);

   tsd::communication::event::IfcAddr_t seqMask =
      ((UINT64_C(1) << m_subNetPrefixLength) - 1u) << newPrefixShift;
   tsd::communication::event::IfcAddr_t newAddr;

   // prevent endless loop if no addresses are free
   uint32_t subNetSeqNumMask = (UINT32_C(1) << m_subNetPrefixLength) - 1u;
   if (m_subNets.size() >= subNetSeqNumMask) {
      m_log << tsd::common::logging::LogLevel::Warn
            << m_name << ": no free downstream address available!"
            << &std::endl;
      return false;
   }

   do {
      // zero could be local host!
      if ((m_subNetSeqNum & subNetSeqNumMask) == 0u) {
         m_subNetSeqNum++;
      }

      newAddr = ourNetAddr | ((static_cast<IfcAddr_t>(m_subNetSeqNum) << newPrefixShift) & seqMask);
      m_subNetSeqNum++;
   } while (m_subNets.find(newAddr) != m_subNets.end());

   m_subNets.insert(newAddr);
   addr = newAddr;
   prefix = newPrefixLength;
   nameServer = newAddr | getNameServer();

   return true;
}

void Router::freeDownstreamAddr(tsd::communication::event::IfcAddr_t address)
{
   tsd::common::system::MutexGuard g(m_lock);
   m_subNets.erase(address);
}

tsd::communication::event::IfcAddr_t Router::getNameServer() const
{
   return m_nameServer->getInterfaceAddr();
}

void Router::addDownstreamPort(IPort *port)
{
   m_log << tsd::common::logging::LogLevel::Debug
         << m_name << ": add downstream port "
         << std::hex << std::setfill('0')
         << std::setw(16) << port->getLocalAddr() << "/"
         << std::setw(16) << port->getMask()
         << std::endl;

   tsd::common::system::MutexGuard g(m_lock);
   m_ports.push_back(port);
}

bool Router::addUpstreamPort(IPort *port,
                             tsd::communication::event::IfcAddr_t nameServer,
                             const std::string &subDomain)
{
   tsd::common::system::MutexGuard g(m_lock);

   // there can only be one upstream port
   if (m_defaultGateway != NULL) {
      return false;
   }

   // TODO: check for collisions
   //  - if a default gateway is added all interfaces must be below our
   //    authorative range
   //  - no common sub-nets between ports
   // At the moment we just prevent upstream connections if there are already
   // downstream connections.
   if (!m_subNets.empty()) {
      return false;
   }

   bool ret = true;
   m_ports.push_back(port);
   m_defaultGateway = port;

   g.unlock();
   if (subDomain.empty()) {
      ret = m_nameServer->makeStubResolver(nameServer);
   } else {
      ret = m_nameServer->makeAuthorative(nameServer, subDomain);
   }
   g.lock();

   if (!ret) {
      delPort(port);
   } else {
      m_log << tsd::common::logging::LogLevel::Debug
            << m_name << ": add upstream port "
            << std::hex << std::setfill('0')
            << std::setw(16) << port->getLocalAddr() << "/"
            << std::setw(16) << port->getMask()
            << std::endl;
   }

   return ret;
}

void Router::delPort(IPort *port)
{
   m_log << tsd::common::logging::LogLevel::Debug
         << m_name << ": remove port "
         << std::hex << std::setfill('0')
         << std::setw(16) << port->getLocalAddr() << "/"
         << std::setw(16) << port->getMask()
         << std::endl;

   tsd::common::system::MutexGuard g(m_lock);

   /*
    * First remove the port from all data structures. Multicast groups are
    * cleaned up last to prevent recursion if multiple ports vanish at the same
    * time.
    */

   m_nameServer->portVanished(port->getLocalAddr(), port->getMask());

   for (Ports::iterator it(m_ports.begin()); it != m_ports.end(); ++it) {
      if (*it == port) {
         m_ports.erase(it);
         break;
      }
   }

   bool wasDefaultGw = false;
   if (m_defaultGateway == port) {
      m_nameServer->makeLocal();
      m_defaultGateway = NULL;
      wasDefaultGw = true;
   }

   /*
    * The port is now invisible on the router. Clear all multicast groups that
    * referenced the port.
    */
   clearGroups(port, wasDefaultGw);
}

/**
 * Check if address matches any port address.
 *
 * The local router is reachable through any number of ports. This method
 * checks if the supplied address matches any of our local addresses of the
 * ports.
 */
bool Router::isAnyPortAddr(tsd::communication::event::IfcAddr_t addr)
{
   tsd::common::system::MutexGuard g(m_lock);

   // upstream
   if (m_defaultGateway != NULL && m_defaultGateway->isPortAddr(addr)) {
      return true;
   }

   // downstream
   for (Ports::const_iterator it(m_ports.begin()); it != m_ports.end(); ++it) {
      if ((*it)->isPortAddr(addr)) {
         return true;
      }
   }

   // local?
   return isLocalAddr(addr);
}

/**
 * Check if there is already an upstream port on the router.
 *
 * If this method returns true a subsequent addUpstreamPort() will fail.
 */
bool Router::hasUpstreamPort()
{
   tsd::common::system::MutexGuard g(m_lock);

   return m_defaultGateway != NULL;
}

bool Router::publishInterface(const std::string &interfaceName,
                              IfcAddr_t address)
{
   return m_nameServer->publishInterface(interfaceName, address, false);
}

void Router::unpublishInterface(const std::string &interfaceName,
                                IfcAddr_t address)
{
   m_nameServer->unpublishInterface(interfaceName, address);
}

bool Router::lookupInterface(const std::string &interfaceName, uint32_t timeout, IfcAddr_t &remoteAddr)
{
   return m_nameServer->lookupInterface(interfaceName, timeout, remoteAddr);
}

void Router::routeLocalEvent(std::auto_ptr<TsdEvent> msg, bool multicast)
{
   tsd::common::system::MutexGuard g(m_lock);

   m_log << tsd::common::logging::LogLevel::Trace
         << "routeLocalEvent(" << m_name << ", " << std::dec << msg->getEventId() << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << msg->getSenderAddr() << " -> "
         << std::setw(16) << msg->getReceiverAddr()
         << std::endl;

   IfcAddr_t dest = msg->getReceiverAddr();
   if (isLocalAddr(dest)) {
      Addr2Queue::const_iterator it(m_addr2Queue.find(dest));
      if (it != m_addr2Queue.end()) {
         it->second->pushMessage(msg, 0, multicast);
      } else {
         m_log << tsd::common::logging::LogLevel::Trace
               << "message lost" << & std::endl;
      }
   } else {
      routePacket(std::auto_ptr<Packet>(new Packet(msg.get(), multicast)));
   }
}

bool Router::routeLocalPacket(std::auto_ptr<Packet> pkt, bool multicast)
{
   tsd::common::system::MutexGuard g(m_lock);

   m_log << tsd::common::logging::LogLevel::Trace
         << "routeLocalPacket(" << m_name << ", " << std::dec << pkt->getType() << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << pkt->getSenderAddr() << " -> "
         << std::setw(16) << pkt->getReceiverAddr()
         << std::endl;

   bool routed = false;
   IfcAddr_t dest = pkt->getReceiverAddr();
   if (isLocalAddr(dest)) {
      Addr2Queue::const_iterator a2q(m_addr2Queue.find(dest));
      if (a2q != m_addr2Queue.end()) {
         routed = a2q->second->pushPacket(pkt, multicast);
      } else {
         m_log << tsd::common::logging::LogLevel::Trace
               << "packet lost" << &std::endl;
      }
   } else {
      routed = routePacket(pkt);
   }

   return routed;
}

IPort* Router::getEgressPort(IfcAddr_t &dest)
{
   IPort *egressPort = NULL;

   // Address mangling
   for (Ports::const_iterator it(m_ports.begin()); it != m_ports.end(); ++it) {
      if ((*it)->isPortAddr(dest)) {
         // local delivery (convert to localhost address)
         dest = getInterface(dest);
         break;
      } else if ((*it)->isPortNet(dest)) {
         egressPort = *it;
         break;
      }
   }

   /*
    * Use default gateway if this is neither local nor adjacent net. But we
    * must not pass the packet upwards if it belongs to this routers subnet!
    */
   if (!isLocalAddr(dest) && (egressPort == NULL) && (m_defaultGateway != NULL)) {
      IfcAddr_t ourAddr = m_defaultGateway->getLocalAddr();
      IfcAddr_t ourMask = ~(~IfcAddr_t(0) >> m_defaultGateway->getPrefixLength());
      if (!isNetworkAddr(dest, ourAddr, ourMask)) {
         egressPort = m_defaultGateway;
      }
   }

   return egressPort;
}

bool Router::isEgressPort(IfcAddr_t dest, IPort *port, bool isDefaultGw)
{
   // delivery on this port?
   if (port->isPortNet(dest)) {
      if (port->isPortAddr(dest)) {
         // local delivery on this port -> would not go outside
         return false;
      } else {
         // remote delivery on this port -> bingo
         return true;
      }
   }

   // Is this packet for the default gateway? The logic must duplicate
   // getEgressPort().
   if (isDefaultGw && !isLocalAddr(dest)) {
      IfcAddr_t ourAddr = port->getLocalAddr();
      IfcAddr_t ourMask = ~(~IfcAddr_t(0) >> port->getPrefixLength());
      if (!isNetworkAddr(dest, ourAddr, ourMask)) {
         return true;
      }
   }

   return false;
}

bool Router::routePacket(std::auto_ptr<Packet> evt, IPort *ingressPort)
{
   tsd::common::system::MutexGuard g(m_lock);

   // Get destination port
   IfcAddr_t dest = evt->getReceiverAddr();
   IPort *egressPort = getEgressPort(dest);

   // No destination -> probably local delivery
   if (egressPort == NULL) {
      evt->setReceiverAddr(dest);
   }

   // destination is reachable on this port
   if (egressPort != NULL && isLocalAddr(evt->getSenderAddr())) {
      // set egress address (keeping interface)
      evt->setSenderAddr(egressPort->getLocalAddr() | evt->getSenderAddr());
   }

   m_log << tsd::common::logging::LogLevel::Trace
         << "routePacket(" << m_name << ", " << std::dec << evt->getType()
         << ", " << evt->getEventId() << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << evt->getSenderAddr() << " -> "
         << std::setw(16) << evt->getReceiverAddr()
         << std::endl;

   /*
    * Special multicast handling.
    *
    * Multicast groups are managed on every hop. Therefore we need to
    * "intercept" join/leave messages and process them locally even if sender
    * and receiver are on remote nodes.
    */
   if (ingressPort != NULL) {
      switch (evt->getType()) {
      case Packet::MULTICAST_JOIN:
         if (!joinGroup(dest, evt->getSenderAddr())) {
            routeDeathMessage(dest, evt->getSenderAddr());
         }
         return true;

      case Packet::MULTICAST_LEAVE:
         leaveGroup(dest, evt->getSenderAddr());
         return true;

      case Packet::DEATH_NOTIFICATION:
         sendDeathMessage(dest, evt->getSenderAddr());
         return true;

      default:
         // ignore other packets
         break;
      }
   }

   bool routed = false;
   if (isLocalAddr(dest)) {
      switch (evt->getType()) {
      case Packet::UNICAST_MESSAGE:
         routed = routeLocalPacket(evt, false);
         break;

      case Packet::MULTICAST_MESSAGE:
      {
         MulticastGroups::const_iterator group(m_multicastGroups.find(dest));
         if (group != m_multicastGroups.end()) {
            IfcAddr_t found = LOOPBACK_ADDRESS;

            /*
             * Make a copy of the multicast observers. Routing a packet might
             * evict the destination port because it was recognized as dead
             * which will prune m_multicastGroups in delPort(). In this case we
             * might try to forward packets to non-existent addresses which is
             * ok as it will simply drop them.
             */
            MulticastReceivers observers(group->second.m_receivers);
            for (MulticastReceivers::const_iterator it(observers.begin()); it != observers.end(); ++it) {
               if (found != LOOPBACK_ADDRESS) {
                  std::auto_ptr<Packet> tmp(new Packet(*(evt.get())));
                  tmp->setReceiverAddr(found);
                  routeLocalPacket(tmp, true);
               }
               found = *it;
            }

            if (found != LOOPBACK_ADDRESS) {
               evt->setReceiverAddr(found);
               routeLocalPacket(evt, true);
               routed = true;
            }
         }
         break;
      }

      default:
         // ignore other packets
         break;
      }
   } else {
      if (egressPort != NULL) {
         routed = egressPort->sendPacket(evt);
      }
   }

   return routed;
}

void Router::sendUnicastMessage(IfcAddr_t localAddr, IfcAddr_t remoteAddr, std::auto_ptr<TsdEvent> msg)
{
   msg->setSenderAddr(localAddr);
   msg->setReceiverAddr(remoteAddr);
   routeLocalEvent(msg, false);
}

void Router::routeBroadcastMessage(IfcAddr_t localAddr, IfcAddr_t remoteAddr, std::auto_ptr<TsdEvent> msg)
{
   m_log << tsd::common::logging::LogLevel::Trace
         << "routeBroadcastMessage(" << m_name << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << localAddr << " -> "
         << std::setw(16) << remoteAddr << ", "
         << std::dec << msg->getEventId()
         << std::endl;

   msg->setSenderAddr(localAddr);
   msg->setReceiverAddr(remoteAddr);
   routeLocalEvent(msg, true);
}

bool Router::joinGroup(IfcAddr_t sender, IfcAddr_t receiver)
{
   tsd::common::system::MutexGuard g(m_lock);

   m_log << tsd::common::logging::LogLevel::Debug
         << "joinGroup(" << m_name << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << sender << " -> "
         << std::setw(16) << receiver
         << std::endl;

   bool alive;
   if (isLocalAddr(sender)) {
      alive = m_addr2Queue.find(sender) != m_addr2Queue.end();
      m_multicastGroups[sender].m_receivers.push_back(receiver);
      m_multicastGroups[sender].m_alive = alive;
   } else {
      MulticastStubs::const_iterator it(m_multicastStubs.find(sender));
      if (it != m_multicastStubs.end()) {
         // already stubbed
         m_multicastGroups[it->second].m_receivers.push_back(receiver);
         alive = m_multicastGroups[it->second].m_alive;
      } else {
         IfcAddr_t stub = allocateIfcAddrInternal();
         m_multicastStubs[sender] = stub;
         // if packet is gets lost the group is dead
         alive = routePacket(std::auto_ptr<Packet>(
            new Packet(Packet::MULTICAST_JOIN, stub, sender)));
         m_multicastGroups[stub].m_receivers.push_back(receiver);
         m_multicastGroups[stub].m_alive = alive;
      }
   }

   return alive;
}

bool Router::leaveGroup(IfcAddr_t sender, IfcAddr_t receiver)
{
   bool ret = false;
   tsd::common::system::MutexGuard g(m_lock);

   m_log << tsd::common::logging::LogLevel::Debug
         << "leaveGroup(" << m_name << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << sender << " -> "
         << std::setw(16) << receiver
         << std::endl;

   if (isLocalAddr(sender)) {
      MulticastReceivers &observers = m_multicastGroups[sender].m_receivers;
      MulticastReceivers::iterator it(observers.begin());
      while (it != observers.end()) {
         if (*it == receiver) {
            it = observers.erase(it);
         } else {
            ++it;
         }
      }

      ret = observers.empty();
      if (ret) {
         m_multicastGroups.erase(sender);
      }
   } else {
      MulticastStubs::iterator it(m_multicastStubs.find(sender));
      if (it != m_multicastStubs.end()) {
         IfcAddr_t stub = it->second;
         ret = leaveGroup(stub, receiver);
         if (ret) {
            m_multicastStubs.erase(it);
            freeIfcAddrInternal(stub);
            routePacket(std::auto_ptr<Packet>(
               new Packet(Packet::MULTICAST_LEAVE, stub, sender)));
         }
      }
   }

   return ret;
}

void Router::clearGroups(IPort *oldPort, bool wasDefaultGw)
{
   typedef std::list<Packet*> PacketQueue;
   typedef std::list< std::pair<IfcAddr_t, IfcAddr_t> > DeathQueue;

   /*
    * Check outgoing multicast groups. These are groups that have been
    * registered by someone on the vanishing port. If we find such groups we
    * have to free them because we won't get a leaveGroup() for them.
    */
   PacketQueue leaveQueue;
   MulticastGroups::iterator mgIt(m_multicastGroups.begin());
   while (mgIt != m_multicastGroups.end()) {
      // check if any observer is on vanishing port
      MulticastReceivers &observers = mgIt->second.m_receivers;
      MulticastReceivers::iterator obsIt(observers.begin());
      while (obsIt != observers.end()) {
         IfcAddr_t dest = *obsIt;
         if (isEgressPort(dest, oldPort, wasDefaultGw)) {
            m_log << tsd::common::logging::LogLevel::Debug
                  << m_name << ": remove remote receiver "
                  << std::hex << std::setfill('0')
                  << std::setw(16) << dest <<  " on "
                  << std::setw(16) << oldPort->getLocalAddr() << "/"
                  << std::setw(16) << oldPort->getMask()
                  << std::endl;

            obsIt = observers.erase(obsIt);
         } else {
            ++obsIt;
         }
      }

      if (observers.empty()) {
         // group is now empty. May have been a stubbed group
         MulticastStubs::iterator stubsIt(m_multicastStubs.begin());
         while (stubsIt != m_multicastStubs.end()) {
            if (stubsIt->second == mgIt->first) {
               m_log << tsd::common::logging::LogLevel::Debug
                     << m_name << ": remove stub for "
                     << std::hex << std::setfill('0')
                     << std::setw(16) << stubsIt->first
                     << std::endl;
               freeIfcAddrInternal(stubsIt->second);
               leaveQueue.push_back(
                  new Packet(Packet::MULTICAST_LEAVE, stubsIt->second, stubsIt->first));
               MulticastStubs::iterator tmp(stubsIt++);
               m_multicastStubs.erase(tmp);
            } else {
               ++stubsIt;
            }
         }
         // delete group
         m_log << tsd::common::logging::LogLevel::Debug
               << m_name << ": delete empty group "
               << std::hex << std::setfill('0')
               << std::setw(16) << mgIt->first
               << std::endl;
         MulticastGroups::iterator tmp(mgIt++);
         m_multicastGroups.erase(tmp);
      } else {
         ++mgIt;
      }
   }

   /*
    * Check incoming multicast groups. These groups are still managed by
    * someone, but they won't receive any messages anymore. This will trigger
    * local monitors and send a death notification to all subscribers.
    */
   DeathQueue deathQueue;
   for (MulticastStubs::iterator it(m_multicastStubs.begin()); it != m_multicastStubs.end(); ++it) {
      IfcAddr_t sender = it->first;
      if (isEgressPort(sender, oldPort, wasDefaultGw)) {
         deathQueue.push_back(std::make_pair(it->second, sender));
      }
   }

   /*
    * Finally send all "multicast leave" and "death" messages. This has to be
    * done at the end because their delivery might trigger further port
    * deletions which may invalidate our iterators above.
    */
   for (PacketQueue::iterator it(leaveQueue.begin()); it != leaveQueue.end(); ++it) {
      routePacket(std::auto_ptr<Packet>(*it));
   }
   for (DeathQueue::iterator it(deathQueue.begin()); it != deathQueue.end(); ++it) {
      sendDeathMessage(it->first, it->second);
   }
}

void Router::sendBroadcastMessage(IfcAddr_t localAddr, std::auto_ptr<TsdEvent> msg)
{
   tsd::common::system::MutexGuard g(m_lock);

   IfcAddr_t found = LOOPBACK_ADDRESS;
   MulticastGroups::const_iterator group(m_multicastGroups.find(localAddr));
   if (group != m_multicastGroups.end()) {
      /*
       * Make a copy of the multicast observers. Routing a packet might modify
       * m_multicastGroups which invalidates our iterator. We don't care about
       * vanishing ports here because forwarding packets to non-existent
       * addresses which is ok as the router will simply drop them.
       */
      MulticastReceivers observers(group->second.m_receivers);

      for (MulticastReceivers::const_iterator it(observers.begin()); it != observers.end(); ++it) {
         if (found != LOOPBACK_ADDRESS) {
            routeBroadcastMessage(localAddr, found, std::auto_ptr<TsdEvent>(msg->clone()));
         }
         found = *it;
      }

      if (found != LOOPBACK_ADDRESS) {
         routeBroadcastMessage(localAddr, found, msg);
      }
   }

   if (found == LOOPBACK_ADDRESS) {
      m_log << tsd::common::logging::LogLevel::Trace
            << m_name << ": sendBroadcastMessage: lost event " << msg->getEventId()
            << &std::endl;
   }
}

void Router::sendDeathMessage(IfcAddr_t localAddr, IfcAddr_t source)
{
   typedef std::list< std::pair<IfcAddr_t, IfcAddr_t> > DeathQueue;

   DeathQueue deathQueue;
   MulticastGroups::iterator group(m_multicastGroups.find(localAddr));
   if (group != m_multicastGroups.end()) {
      group->second.m_alive = false;
      const MulticastReceivers &observers = group->second.m_receivers;
      for (MulticastReceivers::const_iterator it(observers.begin()); it != observers.end(); ++it) {
         deathQueue.push_back(std::make_pair(source, *it));
      }
   }

   for (DeathQueue::iterator it(deathQueue.begin()); it != deathQueue.end(); ++it) {
      routeDeathMessage(it->first, it->second);
   }
}

void Router::routeDeathMessage(IfcAddr_t sender, IfcAddr_t receiver)
{
   m_log << tsd::common::logging::LogLevel::Trace
         << "routeDeathMessage(" << m_name << "): "
         << std::hex << std::setfill('0')
         << std::setw(16) << sender << " -> "
         << std::setw(16) << receiver
         << std::endl;

   if (isLocalAddr(receiver)) {
      Addr2Queue::const_iterator it(m_addr2Queue.find(receiver));
      if (it != m_addr2Queue.end()) {
         it->second->dead(sender, receiver);
      }
   } else {
      routePacket(std::auto_ptr<Packet>(
         new Packet(Packet::DEATH_NOTIFICATION, sender, receiver)));
   }
}



/**
 * Global instance for local address space.
 */

Router* Router::s_LocalRouter = NULL;
tsd::common::system::Mutex Router::s_LocalRouterLock;

Router& Router::getLocalRouter()
{
   tsd::common::system::MutexGuard g(s_LocalRouterLock);

   if (s_LocalRouter == NULL) {
      s_LocalRouter = new Router("localhost");
   }

   return *s_LocalRouter;
}

} } }

