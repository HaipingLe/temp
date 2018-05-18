/**
 *
 *
 * TODO:
 *    * register names upstream when getting a stub resolver
 *    * clean up statele entries when networks vanish (may be multi-hop)
 */
#include <iomanip>

#include <tsd/common/system/Clock.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/communication/messaging/Queue.hpp>

#include "NameServer.hpp"
#include "QueueInternal.hpp"
#include "Router.hpp"
#include "utils.hpp"

#define CACHE_TIMEOUT 1000

using tsd::communication::event::IfcAddr_t;
using tsd::communication::event::TsdEvent;
using tsd::communication::event::LOOPBACK_ADDRESS;

namespace {

   void split(std::vector<std::string> &tokens, const std::string &text, const char sep)
   {
      std::string::size_type start = 0, end = 0;

      tokens.clear();
      while ((end = text.find(sep, start)) != std::string::npos) {
         tokens.push_back(text.substr(start, end - start));
         start = end + 1;
      }

      tokens.push_back(text.substr(start));
   }

}

namespace tsd { namespace communication { namespace messaging {

/*
 * NameServer protocol
 *
 * FIXME: add cookie for request<>reply matching
 */

static const uint32_t QUIT_IND = 1;
static const uint32_t REGISTER_NAME_REQ = 2;
static const uint32_t REGISTER_NAME_REPLY = 3;
static const uint32_t DEREGISTER_NAME_REQ = 4;
//static const uint32_t DEREGISTER_NAME_REPLY = 5;
static const uint32_t QUERY_NAME_REQ = 6;
static const uint32_t QUERY_NAME_REPLY = 7;
static const uint32_t REDIRECT_REPLY = 8;

static const uint32_t NAME_REGISTERED_IND = 16;

class QuitInd
   : public TsdEvent
{
public:
   QuitInd() : TsdEvent(QUIT_IND) { }
   void serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const { }
   void deserialize(tsd::common::ipc::RpcBuffer& /*buf*/) { }
   TsdEvent* clone(void) const { return new QuitInd; }
};

class RegisterNameReq
   : public TsdEvent
{
   std::string m_interfaceName;
   IfcAddr_t m_address;
   bool m_isNameServer;

public:
   RegisterNameReq()
      : TsdEvent(REGISTER_NAME_REQ)
      , m_address(0)
      , m_isNameServer(false)
   { }

   RegisterNameReq(const std::string &interfaceName, IfcAddr_t address, bool isNameServer)
      : TsdEvent(REGISTER_NAME_REQ)
      , m_interfaceName(interfaceName)
      , m_address(address)
      , m_isNameServer(isNameServer)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_interfaceName;
      buf << m_address;
      buf << m_isNameServer;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_interfaceName;
      buf >> m_address;
      buf >> m_isNameServer;
   }

   TsdEvent *clone() const
   {
      return new RegisterNameReq(m_interfaceName, m_address, m_isNameServer);
   }

   std::string interfaceName() const
   {
      return m_interfaceName;
   }

   IfcAddr_t address() const
   {
      return m_address;
   }

   bool isNameServer() const
   {
      return m_isNameServer;
   }
};

class RegisterNameReply
   : public TsdEvent
{
   bool m_success;
   std::string m_prefix;

public:
   RegisterNameReply()
      : TsdEvent(REGISTER_NAME_REPLY)
      , m_success(false)
   { }

   RegisterNameReply(bool success, const std::string &prefix)
      : TsdEvent(REGISTER_NAME_REPLY)
      , m_success(success)
      , m_prefix(prefix)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_success;
      buf << m_prefix;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_success;
      buf >> m_prefix;
   }

   TsdEvent *clone() const
   {
      return new RegisterNameReply(m_success, m_prefix);
   }

   bool success() const { return m_success; }
   std::string prefix() const { return m_prefix; }
};

class DeregisterNameReq
   : public TsdEvent
{
   std::string m_interfaceName;
   IfcAddr_t m_address;

public:
   DeregisterNameReq()
      : TsdEvent(DEREGISTER_NAME_REQ)
      , m_address(0)
   { }

   DeregisterNameReq(const std::string &interfaceName, IfcAddr_t address)
      : TsdEvent(DEREGISTER_NAME_REQ)
      , m_interfaceName(interfaceName)
      , m_address(address)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_interfaceName;
      buf << m_address;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_interfaceName;
      buf >> m_address;
   }

   TsdEvent *clone() const
   {
      return new DeregisterNameReq(m_interfaceName, m_address);
   }

   std::string interfaceName() const
   {
      return m_interfaceName;
   }

   IfcAddr_t address() const
   {
      return m_address;
   }
};

class QueryNameReq
   : public TsdEvent
{
   std::string m_interfaceName;

public:
   QueryNameReq()
      : TsdEvent(QUERY_NAME_REQ)
   { }

   QueryNameReq(const std::string &interfaceName)
      : TsdEvent(QUERY_NAME_REQ)
      , m_interfaceName(interfaceName)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_interfaceName;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_interfaceName;
   }

   TsdEvent *clone() const
   {
      return new QueryNameReq(m_interfaceName);
   }

   std::string interfaceName() const
   {
      return m_interfaceName;
   }
};

class QueryNameReply
   : public TsdEvent
{
   bool m_success;
   bool m_isNameServer;
   IfcAddr_t m_address;

public:
   QueryNameReply()
      : TsdEvent(QUERY_NAME_REPLY)
      , m_success(false)
      , m_isNameServer(false)
      , m_address(0)
   { }

   QueryNameReply(bool success, bool isNameServer, IfcAddr_t address)
      : TsdEvent(QUERY_NAME_REPLY)
      , m_success(success)
      , m_isNameServer(isNameServer)
      , m_address(address)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_success;
      buf << m_isNameServer;
      buf << m_address;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_success;
      buf >> m_isNameServer;
      buf >> m_address;
   }

   TsdEvent *clone() const
   {
      return new QueryNameReply(m_success, m_isNameServer, m_address);
   }

   bool success() const
   {
      return m_success;
   }

   IfcAddr_t address() const
   {
      return m_address;
   }

   bool isNameServer() const
   {
      return m_isNameServer;
   }
};

class RedirectReply
   : public TsdEvent
{
   IfcAddr_t m_address;

public:
   RedirectReply()
      : TsdEvent(REDIRECT_REPLY)
      , m_address(0)
   { }

   RedirectReply(IfcAddr_t address)
      : TsdEvent(REDIRECT_REPLY)
      , m_address(address)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_address;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_address;
   }

   TsdEvent *clone() const
   {
      return new RedirectReply(m_address);
   }

   IfcAddr_t address() const
   {
      return m_address;
   }
};

class NameRegisteredInd
   : public TsdEvent
{
public:
   NameRegisteredInd() : TsdEvent(NAME_REGISTERED_IND) { }
   void serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const { }
   void deserialize(tsd::common::ipc::RpcBuffer& /*buf*/) { }
   TsdEvent* clone(void) const { return new NameRegisteredInd; }
};

class NameServerProtocol
   : public IMessageFactory
{
   std::auto_ptr<TsdEvent> createEvent(uint32_t msgId) const
   {
      std::auto_ptr<TsdEvent> ret;
      switch (msgId) {
         case QUIT_IND:
            ret.reset(new QuitInd);
            break;
         case REGISTER_NAME_REQ:
            ret.reset(new RegisterNameReq);
            break;
         case REGISTER_NAME_REPLY:
            ret.reset(new RegisterNameReply);
            break;
         case DEREGISTER_NAME_REQ:
            ret.reset(new DeregisterNameReq);
            break;
         case QUERY_NAME_REQ:
            ret.reset(new QueryNameReq);
            break;
         case QUERY_NAME_REPLY:
            ret.reset(new QueryNameReply);
            break;
         case REDIRECT_REPLY:
            ret.reset(new RedirectReply);
            break;
         case NAME_REGISTERED_IND:
            ret.reset(new NameRegisteredInd);
            break;
      };

      return ret;
   }

   std::vector<uint32_t> getEventIds()
   {
      std::vector<uint32_t> ret;
      ret.push_back(QUIT_IND);
      ret.push_back(REGISTER_NAME_REQ);
      ret.push_back(REGISTER_NAME_REPLY);
      ret.push_back(DEREGISTER_NAME_REQ);
      ret.push_back(QUERY_NAME_REQ);
      ret.push_back(QUERY_NAME_REPLY);
      ret.push_back(REDIRECT_REPLY);
      ret.push_back(NAME_REGISTERED_IND);
      return ret;
   }
};

NameServerProtocol nsProtocol;

/*
 * States:
 *    - standalone nameserver
 *    - stub resolver
 *    - authorative nameserver
 */

NameServer::NameServer(Router &router)
   : tsd::common::system::Thread(router.getName())
   , m_router(router)
   , m_log("tsd.communication.messaging.nameserver")
   , m_queue(new Queue("ns-srv", m_router))
   , m_ifc(m_queue->registerInterface(nsProtocol))
   , m_upstreamNameServer(LOOPBACK_ADDRESS)
   , m_stubResolver(false)
{
   m_authDomainVec.push_back("");
}

NameServer::~NameServer()
{
}

void NameServer::init()
{
   start();
}

void NameServer::fini()
{
   m_queue->sendSelfMessage(std::auto_ptr<TsdEvent>(new QuitInd));
   join();
}

IfcAddr_t NameServer::getInterfaceAddr() const
{
   return m_ifc->getLocalIfcAddr();
}

void
NameServer::run()
{
   bool running = true;

   while (running) {
      std::auto_ptr<TsdEvent> msg = m_queue->readMessage();

      switch (msg->getEventId()) {
         case QUIT_IND:
            running = false;
            break;

         case REGISTER_NAME_REQ:
         {
            RegisterNameReq *req = dynamic_cast<RegisterNameReq*>(msg.get());
            IfcAddr_t addr = req->address();

            m_log << tsd::common::logging::LogLevel::Debug
                  << m_router.getName() << ": <REG("
                  << req->interfaceName() << "@"
                  << std::hex << std::setfill('0') << std::setw(16) << addr << ")" << &std::endl;

            if (m_stubResolver) {
               m_log << tsd::common::logging::LogLevel::Debug
                     << m_router.getName() << ": >REG(redirect to "
                     << std::hex << std::setfill('0') << std::setw(16) << m_upstreamNameServer << ")" << &std::endl;
               m_ifc->sendMessage(msg->getSenderAddr(),
                  std::auto_ptr<TsdEvent>(new RedirectReply(m_upstreamNameServer)));
            } else {
               if (isLocalAddr(addr)) {
                  // use public host address from where it came
                  addr |= getHostAddr(req->getSenderAddr());
               }

               bool success = publishInterfaceLocal(req->interfaceName(), addr, req->isNameServer());

               m_log << tsd::common::logging::LogLevel::Debug
                     << m_router.getName() << ": >REG("
                     << (success ? "true" : "false") << ", "
                     << std::hex << std::setfill('0') << std::setw(16) << addr << ")"
                     << &std::endl;

               m_ifc->sendMessage(msg->getSenderAddr(),
                  std::auto_ptr<TsdEvent>(new RegisterNameReply(success, m_authDomainStr)));
            }
            break;
         }

         case DEREGISTER_NAME_REQ:
         {
            DeregisterNameReq *req = dynamic_cast<DeregisterNameReq*>(msg.get());
            IfcAddr_t addr = req->address();
            if (isLocalAddr(addr)) {
               // use public host address from where it came
               addr |= getHostAddr(req->getSenderAddr());
            }

            m_log << tsd::common::logging::LogLevel::Debug
                  << m_router.getName() << ": DEREG("
                  << req->interfaceName() << "@"
                  << std::hex << std::setfill('0') << std::setw(16) << addr << ")" << &std::endl;

            unpublishInterface(req->interfaceName(), addr);
            break;
         }

         case QUERY_NAME_REQ:
         {
            QueryNameReq *req = dynamic_cast<QueryNameReq*>(msg.get());
            m_log << tsd::common::logging::LogLevel::Debug
                  << m_router.getName() << ": <QUERY("
                  << req->interfaceName() << ")" << &std::endl;

            if (m_stubResolver) {
               m_log << tsd::common::logging::LogLevel::Debug
                     << m_router.getName() << ": >QUERY(redirect to "
                     << std::hex << std::setfill('0') << std::setw(16) << m_upstreamNameServer << ")" << &std::endl;
               m_ifc->sendMessage(msg->getSenderAddr(),
                  std::auto_ptr<TsdEvent>(new RedirectReply(m_upstreamNameServer)));
            } else {
               bool success = false;
               bool redirect = false;
               bool isNameServer = false;

               IfcAddr_t addr = queryInterface(req->interfaceName(), success, redirect, isNameServer);
               if (success && redirect) {
                  m_log << tsd::common::logging::LogLevel::Debug
                        << m_router.getName() << ": >QUERY(redirect to "
                        << std::hex << std::setfill('0') << std::setw(16) << addr << ")" << &std::endl;
                  m_ifc->sendMessage(msg->getSenderAddr(),
                     std::auto_ptr<TsdEvent>(new RedirectReply(addr)));
               } else {
                  m_log << tsd::common::logging::LogLevel::Debug
                        << m_router.getName() << ": >QUERY("
                        << (success ? "true" : "false") << ", "
                        << std::hex << std::setfill('0') << std::setw(16) << addr << ")" << &std::endl;
                  m_ifc->sendMessage(msg->getSenderAddr(),
                     std::auto_ptr<TsdEvent>(new QueryNameReply(success, isNameServer, addr)));
               }
            }
            break;
         }
      }
   }
}

bool
NameServer::publishInterface(const std::string &interfaceName,
                             tsd::communication::event::IfcAddr_t address,
                             bool isNameServer)
{
   m_log << tsd::common::logging::LogLevel::Info
         << m_router.getName() << ": publishInterface(" << (m_stubResolver ? "STUB, " : "LOCAL/AUTH, ")
         << interfaceName << ", " << (isNameServer ? "ns" : "ifc") << ", "
         << std::hex << std::setfill('0') << std::setw(16) << address << ")" << &std::endl;

   tsd::common::system::MutexGuard g(m_lock);
   bool stubResolver = m_stubResolver;
   g.unlock();

   bool ret;
   if (stubResolver) {
      ret = publishInterfaceLocal(interfaceName, address, isNameServer);
      if (ret) {
         ret = publishInterfaceUpstream(interfaceName, address, isNameServer, NULL);
         if (!ret) {
            unpublishInterfaceLocal(interfaceName, address);
         }
      }
   } else {
      ret = publishInterfaceLocal(interfaceName, address, isNameServer);
   }

   return ret;
}

bool
NameServer::publishInterfaceLocal(const std::string &interfaceName,
                                  tsd::communication::event::IfcAddr_t address,
                                  bool isNameServer)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_interfaces.find(interfaceName) != m_interfaces.end())
      return false;

   Entry &e = m_interfaces[interfaceName];
   e.m_addr = address;
   e.m_isNameServer = isNameServer;
   g.unlock();
   m_ifc->broadcastMessage(std::auto_ptr<TsdEvent>(new NameRegisteredInd));

   return true;
}

bool
NameServer::publishInterfaceUpstream(const std::string &interfaceName,
                                     tsd::communication::event::IfcAddr_t address,
                                     bool isNameServer,
                                     std::string *prefix)
{
   tsd::common::system::MutexGuard g(m_lock);
   std::auto_ptr<Queue> q(new Queue("ns-publish", m_router));
   IfcAddr_t nameServer = m_upstreamNameServer;
   g.unlock();

   std::auto_ptr<IRemoteIfc> ifc(q->connectInterface(nameServer, nsProtocol));
   ifc->sendMessage(std::auto_ptr<TsdEvent>(
      new RegisterNameReq(interfaceName, address, isNameServer)));

   bool done = false;
   bool success = false;
   do {
      std::auto_ptr<TsdEvent> m = q->readMessage(); // FIXME: monitor

      switch (m->getEventId()) {
         case REGISTER_NAME_REPLY:
         {
            RegisterNameReply *reply = dynamic_cast<RegisterNameReply*>(m.get());
            success = reply->success();
            if (success && prefix != NULL) {
               *prefix = reply->prefix();
            }
            done = true;
            break;
         }
         case REDIRECT_REPLY:
         {
            RedirectReply *reply = dynamic_cast<RedirectReply*>(m.get());
            nameServer = reply->address();
            ifc.reset(q->connectInterface(nameServer, nsProtocol));
            ifc->sendMessage(std::auto_ptr<TsdEvent>(
               new RegisterNameReq(interfaceName, address, isNameServer)));
            break;
         }
      }
   } while (!done);

   return success;
}

void
NameServer::unpublishInterface(const std::string &interfaceName,
                               tsd::communication::event::IfcAddr_t address)
{
   m_log << tsd::common::logging::LogLevel::Info
         << m_router.getName() << ": unpublishInterface("
         << interfaceName << ", "
         << std::hex << std::setfill('0') << std::setw(16) << address << ")" << &std::endl;

   tsd::common::system::MutexGuard g(m_lock);

   if (m_stubResolver) {
      g.unlock();
      unpublishInterfaceUpstream(interfaceName, address);
      g.lock();
   }
   unpublishInterfaceLocal(interfaceName, address);
}

void
NameServer::unpublishInterfaceLocal(const std::string &interfaceName, IfcAddr_t address)
{
   tsd::common::system::MutexGuard g(m_lock);
   Interfaces::iterator i(m_interfaces.find(interfaceName));
   if (i != m_interfaces.end() && i->second.m_addr == address) {
      m_interfaces.erase(i);
   }
}

void
NameServer::unpublishInterfaceUpstream(const std::string &interfaceName, IfcAddr_t address)
{
   tsd::common::system::MutexGuard g(m_lock);
   std::auto_ptr<Queue> q(new Queue("ns-depublish", m_router));
   IfcAddr_t nameServer = m_upstreamNameServer;
   g.unlock();

   std::auto_ptr<IRemoteIfc> ifc(q->connectInterface(nameServer, nsProtocol));
   ifc->sendMessage(std::auto_ptr<TsdEvent>(
      new DeregisterNameReq(interfaceName, address)));
}

bool
NameServer::lookupInterface(const std::string &interfaceName, uint32_t timeout,
                            tsd::communication::event::IfcAddr_t &remoteAddr)
{
   m_log << tsd::common::logging::LogLevel::Debug
         << m_router.getName() << ": lookupInterface("
         << interfaceName << ")" << &std::endl;

   if (interfaceName.empty()) {
      return false;
   }

   uint32_t startTime = (timeout != INFINITE_TIMEOUT)
      ? tsd::common::system::Clock::getTickCounter()
      : 0;

   tsd::common::system::MutexGuard g(m_lock);

   IfcAddr_t nameServer = getInterfaceAddr();
   bool found = false;
   int redirects = 42;

   std::auto_ptr<Queue> q(new Queue("ns-query@"+interfaceName, m_router));
   g.unlock();
   std::auto_ptr<IRemoteIfc> ifc(q->connectInterface(nameServer, nsProtocol));
   ifc->subscribe(NAME_REGISTERED_IND);
   ifc->sendMessage(std::auto_ptr<TsdEvent>(new QueryNameReq(interfaceName)));

   m_log << tsd::common::logging::LogLevel::Trace
         << m_router.getName() << ": lookupInterface(" << interfaceName << "): ask "
         << std::hex << std::setfill('0') << std::setw(16) << nameServer
         << &std::endl;

   while (!found && redirects > 0) {
      std::auto_ptr<TsdEvent> m = q->readMessage(timeout);
      if (m.get() == NULL) {
         break;
      }

      if (timeout != INFINITE_TIMEOUT) {
         uint32_t endTime = tsd::common::system::Clock::getTickCounter();
         uint32_t elapsed = endTime - startTime;
         if (elapsed > timeout) {
            break; // timeout
         } else {
            startTime = endTime;
            timeout -= elapsed;
         }
      }

      switch (m->getEventId()) {
         case QUERY_NAME_REPLY:
         {
            QueryNameReply *reply = dynamic_cast<QueryNameReply*>(m.get());
            if (reply->success()) {
               remoteAddr = reply->address();
               // use public port address from where the reply came in
               if (isLocalAddr(remoteAddr)) {
                  remoteAddr |= getHostAddr(nameServer);
               }
               found = true;
            } else {
               m_log << tsd::common::logging::LogLevel::Trace
                     << m_router.getName() << ": lookupInterface(" << interfaceName
                     << "): got negative reply"
                     << &std::endl;
            }

            break;
         }
         case REDIRECT_REPLY:
         {
            RedirectReply *reply = dynamic_cast<RedirectReply*>(m.get());
            nameServer = reply->address();
            ifc.reset(q->connectInterface(nameServer, nsProtocol));
            ifc->subscribe(NAME_REGISTERED_IND);
            ifc->sendMessage(std::auto_ptr<TsdEvent>(new QueryNameReq(interfaceName)));
            redirects--;

            m_log << tsd::common::logging::LogLevel::Trace
                  << m_router.getName() << ": lookupInterface(" << interfaceName << "): redirect to "
                  << std::hex << std::setfill('0') << std::setw(16) << nameServer
                  << std::dec << std::setw(0) << ", " << redirects << " tries left"
                  << &std::endl;

            break;
         }
         case NAME_REGISTERED_IND:
            // resend query
            ifc->sendMessage(std::auto_ptr<TsdEvent>(new QueryNameReq(interfaceName)));
            break;
      }
   }

   if (found) {
      /*
       * The remote name server may have returned an address that belongs to our
       * router. Convert to a local address in this case. The multi cast handling
       * relies on the property that non-local addresses are remote and _will_ be
       * routed.
       */
      if (m_router.isAnyPortAddr(remoteAddr)) {
         remoteAddr = getInterface(remoteAddr);
      }

      m_log << tsd::common::logging::LogLevel::Debug
            << m_router.getName() << ": lookupInterface(" << interfaceName
            << "): found " << std::hex << std::setfill('0') << std::setw(16) << remoteAddr
            << &std::endl;
   } else {
      m_log << tsd::common::logging::LogLevel::Debug
            << m_router.getName() << ": lookupInterface("
            << interfaceName << "): not found" << &std::endl;
   }

   return found;
}

tsd::communication::event::IfcAddr_t
NameServer::queryInterface(const std::string &interfaceName, bool &success, bool &redirect, bool &isNameServer)
{
   IfcAddr_t ret = LOOPBACK_ADDRESS;

   tsd::common::system::MutexGuard g(m_lock);

   // Local or fully qualified name?
   if (interfaceName[0] != '/') {
      success = queryInterfaceLocal(interfaceName, ret, isNameServer);
   } else {
      std::vector<std::string> ifcNameVec;
      split(ifcNameVec, interfaceName, '/');

      unsigned i;
      for (i = 0; i < std::min(ifcNameVec.size(), m_authDomainVec.size()); i++) {
         if (ifcNameVec[i] != m_authDomainVec[i]) {
            break;
         }
      }

      // matched whole auth prefix?
      if (i == m_authDomainVec.size()) {
         if (i == ifcNameVec.size()) {
            // asked exactly for this authorative domain -> no valid interface query
            success = false;
         } else {
            // lookup next element locally
            success = queryInterfaceLocal(ifcNameVec[i], ret, isNameServer);
            if (success) {
               // We got something. If this was the last element we're expecting
               // an interface, otherwise it should be a name server.
               if (i+1 == ifcNameVec.size()) {
                  redirect = false;
                  success = success && !isNameServer;
               } else {
                  redirect = true;
                  success = success && isNameServer;
               }
            }
         }
      } else {
         // not under our authorative domain -> pass upwards (if possible)
         if (m_upstreamNameServer != LOOPBACK_ADDRESS) {
            success = true;
            redirect = true;
            ret = m_upstreamNameServer;
         } else {
            success = false;
         }
      }
   }

   return ret;
}

bool
NameServer::queryInterfaceLocal(const std::string &interfaceName,
                                tsd::communication::event::IfcAddr_t &remoteAddr,
                                bool &isNameServer)
{
   bool ret = false;

   Interfaces::iterator i = m_interfaces.find(interfaceName);
   if (i != m_interfaces.end()) {
      ret = true;
      remoteAddr = i->second.m_addr;
      isNameServer = i->second.m_isNameServer;
   }

   return ret;
}

void
NameServer::makeLocal()
{
   tsd::common::system::MutexGuard g(m_lock);

   m_upstreamNameServer = LOOPBACK_ADDRESS;
   m_stubResolver = false;
   m_authDomainStr = "";
   m_authDomainVec.clear();
   m_authDomainVec.push_back("");
}

bool
NameServer::makeStubResolver(tsd::communication::event::IfcAddr_t address)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_stubResolver || m_upstreamNameServer != LOOPBACK_ADDRESS) {
      return false;
   }

   m_upstreamNameServer = address;
   m_stubResolver = true;

   // FIXME: propagate registrations to upstream server

   // let everybody retry as we have a upstream name server now
   g.unlock();
   m_ifc->broadcastMessage(std::auto_ptr<TsdEvent>(new NameRegisteredInd));

   return true;
}

bool
NameServer::makeAuthorative(tsd::communication::event::IfcAddr_t address, const std::string &host)
{
   tsd::common::system::MutexGuard g(m_lock);

   if (m_stubResolver || m_upstreamNameServer != LOOPBACK_ADDRESS) {
      return false;
   }

   m_upstreamNameServer = address;
   std::string prefix;

   g.unlock();
   bool ret = publishInterfaceUpstream(host, getInterfaceAddr(), true, &prefix);
   g.lock();

   if (ret) {
      m_authDomainStr = prefix + "/" + host;
      split(m_authDomainVec, m_authDomainStr, '/');
   } else {
      m_upstreamNameServer = LOOPBACK_ADDRESS;
   }

   // let everybody retry as we have a upstream name server now
   g.unlock();
   m_ifc->broadcastMessage(std::auto_ptr<TsdEvent>(new NameRegisteredInd));

   return ret;
}

void
NameServer::portVanished(IfcAddr_t netaddr, IfcAddr_t netmask)
{
   tsd::common::system::MutexGuard g(m_lock);

   // FIXME: move processing to nameserver thread. Otherwise there might be
   // obsolete registrations lurking in the queue from this port.

   // clear registrations reachable on this port
   Interfaces::iterator it(m_interfaces.begin());
   while (it != m_interfaces.end()) {
      if (isNetworkAddr(it->second.m_addr, netaddr, netmask)) {
         Interfaces::iterator tmp(it++);
         m_interfaces.erase(tmp);
      } else {
         ++it;
      }
   }

   // TODO: inform upstream server, if any
   if (m_stubResolver) {
      g.unlock();
   }
}

} } }
