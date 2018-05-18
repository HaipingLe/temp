#ifndef TSD_COMMUNICATION_MESSAGING_NAMESERVER_HPP
#define TSD_COMMUNICATION_MESSAGING_NAMESERVER_HPP

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace messaging {

class Queue;
class Router;
class ILocalIfc;

class NameServer
   : protected tsd::common::system::Thread
{
   struct Entry {
      tsd::communication::event::IfcAddr_t m_addr;
      bool m_isNameServer;
   };
   typedef std::map<std::string, Entry> Interfaces;

   Router &m_router;
   tsd::common::logging::Logger m_log;
   std::auto_ptr<Queue> m_queue;
   std::auto_ptr<ILocalIfc> m_ifc;

   tsd::common::system::Mutex m_lock;
   Interfaces m_interfaces;

   tsd::communication::event::IfcAddr_t m_upstreamNameServer;
   std::string m_authDomainStr;
   std::vector<std::string> m_authDomainVec;
   bool m_stubResolver;

   tsd::communication::event::IfcAddr_t queryInterface(const std::string &interfaceName,
      bool &success, bool &redirect, bool &isNameServer);
   bool publishInterfaceUpstream(const std::string &interfaceName,
      tsd::communication::event::IfcAddr_t address, bool isNameServer, std::string *prefix);
   bool publishInterfaceLocal(const std::string &interfaceName,
                                  tsd::communication::event::IfcAddr_t address,
                                  bool isNameServer);
   void unpublishInterfaceLocal(const std::string &interfaceName,
                                tsd::communication::event::IfcAddr_t address);
   void unpublishInterfaceUpstream(const std::string &interfaceName,
                                   tsd::communication::event::IfcAddr_t address);
   bool queryInterfaceLocal(const std::string &interfaceName,
                                tsd::communication::event::IfcAddr_t &remoteAddr,
                                bool &isNameServer);

protected:
   void run();

public:
   NameServer(Router &router);
   ~NameServer();

   void init();
   void fini();

   tsd::communication::event::IfcAddr_t getInterfaceAddr() const;

   bool publishInterface(const std::string &interfaceName, tsd::communication::event::IfcAddr_t address, bool isNameServer);
   void unpublishInterface(const std::string &interfaceName, tsd::communication::event::IfcAddr_t address);
   bool lookupInterface(const std::string &interfaceName, uint32_t timeout, tsd::communication::event::IfcAddr_t &remoteAddr);

   void makeLocal();
   bool makeStubResolver(tsd::communication::event::IfcAddr_t address);
   bool makeAuthorative(tsd::communication::event::IfcAddr_t address, const std::string &host);

   void portVanished(tsd::communication::event::IfcAddr_t netaddr,
                     tsd::communication::event::IfcAddr_t netmask);
};

} } }

#endif
