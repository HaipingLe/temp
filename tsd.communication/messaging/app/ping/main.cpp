/**
 * Simple echo server and client application.
 *
 * Start one instance with the "-s" option and then one or more other clients
 * to ping the server.
 */

#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/messaging/AddressInUseException.hpp>
#include <tsd/communication/messaging/ConnectionException.hpp>
#include <tsd/communication/messaging/Connection.hpp>
#include <tsd/communication/messaging/Queue.hpp>

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

namespace {

/*
 * Local interface.
 */

const uint32_t QUIT_IND = 1;

class QuitInd
   : public TsdEvent
{
public:
   QuitInd() : TsdEvent(QUIT_IND) { }
   void serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const { }
   void deserialize(tsd::common::ipc::RpcBuffer& /*buf*/) { }
   TsdEvent* clone(void) const { return new QuitInd; }
};

/*
 * Echo service
 */

// Message codes

const uint32_t ECHO_REQ = 1;
const uint32_t ECHO_REPLY = 2;
const uint32_t ECHO_BEACON = 3;

// Message classes

class EchoReq
   : public TsdEvent
{
   std::string m_msg;

public:
   EchoReq()
      : TsdEvent(ECHO_REQ)
   { }

   EchoReq(const std::string &msg)
      : TsdEvent(ECHO_REQ)
      , m_msg(msg)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_msg;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_msg;
   }

   TsdEvent* clone(void) const
   {
      return new EchoReq(m_msg);
   }

   inline std::string getMsg() const { return m_msg; }
   inline void setMsg(const std::string &msg) { m_msg = msg; }
};

class EchoReply
   : public TsdEvent
{
   std::string m_msg;

public:
   EchoReply()
      : TsdEvent(ECHO_REPLY)
   { }

   EchoReply(const std::string &msg)
      : TsdEvent(ECHO_REPLY)
      , m_msg(msg)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_msg;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_msg;
   }

   TsdEvent* clone(void) const
   {
      return new EchoReply(m_msg);
   }

   inline std::string getMsg() const { return m_msg; }
   inline void setMsg(const std::string &msg) { m_msg = msg; }
};

class EchoBeacon
   : public TsdEvent
{
   std::string m_msg;

public:
   EchoBeacon()
      : TsdEvent(ECHO_BEACON)
   { }

   EchoBeacon(const std::string &msg)
      : TsdEvent(ECHO_BEACON)
      , m_msg(msg)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_msg;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_msg;
   }

   TsdEvent* clone(void) const
   {
      return new EchoBeacon(m_msg);
   }

   inline std::string getMsg() const { return m_msg; }
   inline void setMsg(const std::string &msg) { m_msg = msg; }
};

class EchoMsgFactory
   : public IMessageFactory
{
public:
   std::auto_ptr<TsdEvent> createEvent(uint32_t msgId) const
   {
      std::auto_ptr<TsdEvent> ret;

      switch (msgId) {
         case ECHO_REQ:
            ret.reset(new EchoReq);
            break;
         case ECHO_REPLY:
            ret.reset(new EchoReply);
            break;
         case ECHO_BEACON:
            ret.reset(new EchoBeacon);
            break;
      }

      return ret;
   }

   std::vector<uint32_t> getEventIds()
   {
      std::vector<uint32_t> ret;
      ret.push_back(ECHO_REQ);
      ret.push_back(ECHO_REPLY);
      ret.push_back(ECHO_BEACON);
      return ret;
   }

   static IMessageFactory& getInstance()
   {
      static EchoMsgFactory factory;
      return factory;
   }
};

/*****************************************************************************/

class EchoService
{
   class Impl
      : private tsd::common::system::Thread
   {
      std::auto_ptr<IQueue> m_queue;
      std::auto_ptr<ILocalIfc> m_ifc;

      void run(); // tsd::common::system::Thread

   public:
      Impl();
      ~Impl();

      bool init(const std::string &name);
      void finish();
   };

   Impl *m_impl;

public:
   EchoService();
   ~EchoService();

   bool init(const std::string &name);
};

EchoService::Impl::Impl()
   : tsd::common::system::Thread("EchoService")
{ }

EchoService::Impl::~Impl()
{ }

void EchoService::Impl::run()
{
   bool alive = true;

   while (alive) {
      std::auto_ptr<TsdEvent> msg = m_queue->readMessage(1000);

      if (msg.get() == NULL) {
         m_ifc->broadcastMessage(std::auto_ptr<TsdEvent>(new EchoBeacon("Alive")));
      } else if (msg->getReceiverAddr() == m_ifc->getLocalIfcAddr()) {
         switch (msg->getEventId()) {
            case ECHO_REQ:
            {
               EchoReq *req = dynamic_cast<EchoReq*>(msg.get());
               m_ifc->sendMessage(msg->getSenderAddr(),
                  std::auto_ptr<TsdEvent>(new EchoReply("Hello " + req->getMsg())));
               std::cout.put('.');
               std::cout.flush();
               break;
            }
            default:
               std::cout << "Unknown request: " << msg->getEventId()
                         << &std::endl;
               break;
         }
      } else if (msg->getReceiverAddr() == LOOPBACK_ADDRESS) {
         switch (msg->getEventId()) {
            case QUIT_IND:
               alive = false;
               break;
            default:
               std::cout << "Unknown message on loopback iterface: " << msg->getEventId()
                         << &std::endl;
               break;
         }
      } else {
         std::cout << "Unknown message received" << &std::endl;
      }
   }
}

bool EchoService::Impl::init(const std::string &name)
{
   try {
      m_queue.reset(createQueue("echo"));
      m_ifc.reset(m_queue->registerInterface(EchoMsgFactory::getInstance(), name));
   } catch (AddressInUseException &e) {
      return false;
   }

   start();
   return true;
}

void EchoService::Impl::finish()
{
   m_queue->sendSelfMessage(std::auto_ptr<TsdEvent>(new QuitInd));
   join();
}


EchoService::EchoService()
   : m_impl(0)
{ }

EchoService::~EchoService()
{
   if (m_impl) {
      m_impl->finish();
      delete m_impl;
   }
}

bool EchoService::init(const std::string &name)
{
   m_impl = new Impl();
   return m_impl->init(name);
}


} // namespace


/*****************************************************************************/

static void usage()
{
   std::cout << "Usage: tsd.communication.messaging.app.ping [-s] [-n NUM] [-t TRANSPORT] SERVICE\n"
             << "\nArguments:\n"
             << "  SERVICE       Service name of Ping-Service\n"
             << "\nOptions:\n"
             << "  -b            Beacon listening mode\n"
             << "  -l            Use listenDownstream instead of connectUpstream\n"
             << "  -n NUM        Repeat ping NUM times\n"
             << "  -s            Server mode\n"
             << "  -t TRANSPORT  Connect via TRANSPORT\n"
             << "\n"
             << "By default the application will connect via the default transport to a\n"
             << "router and send a ping request to the specified SERVICE. The server side\n"
             << "can be started by giving the '-s' option.\n"
             << "\n"
             << "The default transport may be changed by specifying a different with the '-t'\n"
             << "option. Normally the app will connect to a router. With '-l' the app will\n"
             << "create a listening port for the transport"
             << &std::endl;
   std::exit(1);
}

int main(int /*argc*/, const char * const *argv)
{
   std::string serviceName;
   std::string transport;
   unsigned long repeats = 1;
   bool server = false;
   bool beacon = false;
   bool listen = false;

   for (const char * const *arg = argv+1; *arg != 0; arg++) {
      if (std::strcmp(*arg, "-t") == 0) {
         arg++; if (*arg == 0) { usage(); }
         transport = *arg;
      } else if (std::strcmp(*arg, "-s") == 0) {
         server = true;
      } else if (std::strcmp(*arg, "-b") == 0) {
         beacon = true;
      } else if (std::strcmp(*arg, "-l") == 0) {
         listen = true;
      } else if (std::strcmp(*arg, "-n") == 0) {
         arg++; if (*arg == 0) { usage(); }
         repeats = std::strtoul(*arg, 0, 0);
         if (repeats == 0 || repeats == ULONG_MAX) { usage(); }
      } else {
         if (!serviceName.empty()) { usage(); }
         serviceName = *arg;
      }
   }

   if (serviceName.empty()) {
      usage();
   }

   if (server) {
      // listen for clients
      std::auto_ptr<IConnection> conn;
      try {
         if (listen) {
            conn.reset(listenDownstream(transport));
         } else {
            conn.reset(connectUpstream(transport, "ping"));
         }
      } catch (ConnectionException &e) {
         std::cerr << "Connection failed: " << e.what() << &std::endl
                   << "Did you forget to add -t tcp://192.168.1.4 ?" << std::endl;
         return 1;
      }

      // start echo server
      EchoService echo;
      if (!echo.init(serviceName)) {
         std::cout << "Echo service init failed!" << &std::endl;
         return 1;
      }

      // wait
      std::cout << "Echo service running. Press ENTER to exit." << &std::endl;
      std::string dummy;
      std::getline(std::cin, dummy);
   } else {
      // connect to server
      std::auto_ptr<IConnection> conn;
      try {
         conn.reset(connectUpstream(transport));
      } catch (ConnectionException &e) {
         std::cerr << "Connection failed: " << e.what() << &std::endl
                   << "Did you forget to add -t tcp://192.168.1.4 ?" << std::endl;
         return 1;
      }

      IQueue *q = createQueue("ping");
      IRemoteIfc *ifc = q->connectInterface(serviceName, EchoMsgFactory::getInstance(), 3000);
      if (ifc == 0) {
         std::cout << "Interface not found: " << serviceName << &std::endl;
         return 2;
      }

      std::string reply;

      if (beacon) {
         std::auto_ptr<TsdEvent> msg;

         do {
            msg = q->readMessage();
            if (msg.get()) {
               EchoBeacon *beacon = dynamic_cast<EchoBeacon*>(msg.get());
               if (beacon) {
                  std::cout << "Beacon: " << beacon->getMsg() << &std::endl;
               } else {
                  std::cout << "Unknown message: " << msg->getEventId() << &std::endl;
               }
            }
         } while (msg.get());

         std::cout << "Timeout waiting for beacon" << &std::endl;
      } else {
         for (unsigned int i = 0; i < repeats; i++) {
            ifc->sendMessage(std::auto_ptr<TsdEvent>(new EchoReq("World")));
            std::auto_ptr<TsdEvent> msg = q->readMessage(300);

            if (msg.get() == 0) {
               std::cout << "Timeout waiting for reply" << &std::endl;
               return 2;
            }

            if (msg->getEventId() == ECHO_REPLY) {
               reply = dynamic_cast<EchoReply*>(msg.get())->getMsg();
            }
         }

         std::cout << "Reply: " << reply << &std::endl;
      }
   }

   return 0;
}
