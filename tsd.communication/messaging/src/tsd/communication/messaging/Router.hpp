#ifndef TSD_COMMUNICATION_MESSAGING_ROUTER_HPP
#define TSD_COMMUNICATION_MESSAGING_ROUTER_HPP

#include <map>
#include <memory>
#include <set>
#include <vector>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Mutex.hpp>

#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/Queue.hpp>

namespace tsd { namespace communication { namespace messaging {

class IPort;
class Queue;
class NameServer;
class Packet;

/**
 * Packet router and distributor
 */
class Router
{
   typedef std::map<tsd::communication::event::IfcAddr_t, Queue*> Addr2Queue;
   typedef std::vector<tsd::communication::event::IfcAddr_t> MulticastReceivers;
   struct MulticastGroup {
      MulticastReceivers m_receivers;
      bool m_alive;

      MulticastGroup()
         : m_alive(true)
      { }
   };
   typedef std::map<tsd::communication::event::IfcAddr_t, MulticastGroup> MulticastGroups;
   typedef std::vector<IPort*> Ports;
   typedef std::set<uint32_t> IfcAddrs;
   typedef std::set<tsd::communication::event::IfcAddr_t> SubNets;

   typedef std::map<tsd::communication::event::IfcAddr_t,
      tsd::communication::event::IfcAddr_t> MulticastStubs;

   std::string m_name;
   tsd::common::logging::Logger m_log;
   tsd::common::system::Mutex m_lock;
   Addr2Queue m_addr2Queue;
   MulticastGroups m_multicastGroups;
   MulticastStubs m_multicastStubs;
   uint32_t m_ifcSeqNum;
   IfcAddrs m_ifcAddrs;
   Ports m_ports;
   IPort *m_defaultGateway;
   uint8_t m_subNetPrefixLength;
   uint32_t m_subNetSeqNum;
   SubNets m_subNets;
   std::auto_ptr<NameServer> m_nameServer;

   static Router* s_LocalRouter;
   static tsd::common::system::Mutex s_LocalRouterLock;

   tsd::communication::event::IfcAddr_t allocateIfcAddrInternal();
   void freeIfcAddrInternal(tsd::communication::event::IfcAddr_t address);

   void routeLocalEvent(std::auto_ptr<tsd::communication::event::TsdEvent> msg, bool multicast);
   bool routeLocalPacket(std::auto_ptr<Packet> pkt, bool multicast);
   void routeBroadcastMessage(tsd::communication::event::IfcAddr_t localAddr, tsd::communication::event::IfcAddr_t remoteAddr, std::auto_ptr<tsd::communication::event::TsdEvent> msg);

   IPort* getEgressPort(tsd::communication::event::IfcAddr_t &dest);
   bool isEgressPort(tsd::communication::event::IfcAddr_t dest, IPort *port, bool isDefaultGw);
   void clearGroups(IPort *oldPort, bool wasDefaultGw);

   void sendDeathMessage(tsd::communication::event::IfcAddr_t localAddr, tsd::communication::event::IfcAddr_t source);
   void routeDeathMessage(tsd::communication::event::IfcAddr_t sender, tsd::communication::event::IfcAddr_t receiver);

public:
   Router(const std::string &name);
   ~Router();

   static Router& getLocalRouter();

   // address handling
   tsd::communication::event::IfcAddr_t allocateIfcAddr(Queue *queue);
   void freeIfcAddr(tsd::communication::event::IfcAddr_t address);
   bool setSubnetConfig(uint8_t prefixLength);
   tsd::communication::event::IfcAddr_t getNameServer() const;

   // ports
   bool allocateDownstreamAddr(tsd::communication::event::IfcAddr_t &addr,
      uint8_t &prefix,
      tsd::communication::event::IfcAddr_t &nameServer);
   void freeDownstreamAddr(tsd::communication::event::IfcAddr_t address);
   void addDownstreamPort(IPort *port);
   bool addUpstreamPort(IPort *port,
      tsd::communication::event::IfcAddr_t nameServer,
      const std::string &subDomain);
   void delPort(IPort *port);
   bool isAnyPortAddr(tsd::communication::event::IfcAddr_t addr);
   bool hasUpstreamPort();

   // public name registration
   bool publishInterface(const std::string &interfaceName, tsd::communication::event::IfcAddr_t address);
   void unpublishInterface(const std::string &interfaceName, tsd::communication::event::IfcAddr_t address);
   bool lookupInterface(const std::string &interfaceName, uint32_t timeout, tsd::communication::event::IfcAddr_t &remoteAddr);

   // multicast
   bool joinGroup(tsd::communication::event::IfcAddr_t sender, tsd::communication::event::IfcAddr_t receiver);
   bool leaveGroup(tsd::communication::event::IfcAddr_t sender, tsd::communication::event::IfcAddr_t receiver);

   // routing
   bool routePacket(std::auto_ptr<Packet> evt, IPort *ingressPort = NULL);
   void sendUnicastMessage(tsd::communication::event::IfcAddr_t localAddr, tsd::communication::event::IfcAddr_t remoteAddr, std::auto_ptr<tsd::communication::event::TsdEvent> msg);
   void sendBroadcastMessage(tsd::communication::event::IfcAddr_t localAddr, std::auto_ptr<tsd::communication::event::TsdEvent> msg);

   // misc
   inline std::string getName() const { return m_name; }
};

} } }

#endif
