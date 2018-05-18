#include <iostream>

#include <tsd/common/system/Semaphore.hpp>
#include <tsd/common/system/Thread.hpp>

#include <tsd/communication/messaging/Queue.hpp>

using namespace tsd::communication::event;
using namespace tsd::communication::messaging;

/*
 * Local interface.
 */

static const uint32_t QUIT_IND = 1;

class QuitInd
   : public TsdEvent
{
public:
   QuitInd() : TsdEvent(QUIT_IND) { }
   void serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const { }
   void deserialize(tsd::common::ipc::RpcBuffer& /*buf*/) { }
   TsdEvent* clone(void) const { return new QuitInd; }
};

/******************************************************************************
 * Tuner interface
 *****************************************************************************/

/********************
 * Abstract interface
 */

class ITuner
{
public:
   virtual std::string getVersion() = 0;
};

class ITunerAsyncRequest
{
public:
   virtual void getVersionReq() = 0;
};

class ITunerAsyncReply
{
public:
   virtual void getVersionReply(std::string ret) = 0;
};

/*******************
 * Messaging mapping
 */

// Message codes

static const uint32_t GET_VERSION_REQ = 1;
static const uint32_t GET_VERSION_REPLY = 2;

// Message classes

class GetVersionReq
   : public TsdEvent
{
public:
   GetVersionReq()
      : TsdEvent(GET_VERSION_REQ)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const
   { }

   void deserialize(tsd::common::ipc::RpcBuffer& /*buf*/)
   { }

   TsdEvent* clone(void) const
   {
      return new GetVersionReq;
   }
};

class GetVersionReply
   : public TsdEvent
{
   std::string m_version;

public:
   GetVersionReply()
      : TsdEvent(GET_VERSION_REPLY)
   { }

   GetVersionReply(const std::string &version)
      : TsdEvent(GET_VERSION_REPLY)
      , m_version(version)
   { }

   void serialize(tsd::common::ipc::RpcBuffer& buf) const
   {
      buf << m_version;
   }

   void deserialize(tsd::common::ipc::RpcBuffer& buf)
   {
      buf >> m_version;
   }

   TsdEvent* clone(void) const
   {
      return new GetVersionReply(m_version);
   }

   inline std::string getVersion() const { return m_version; }
   inline void setVersion(const std::string &version) { m_version = version; }
};

class TunerMsgFactory
   : public IMessageFactory
{
public:
   std::auto_ptr<TsdEvent> createEvent(uint32_t msgId) const
   {
      std::auto_ptr<TsdEvent> ret;

      switch (msgId) {
         case GET_VERSION_REQ:
            ret.reset(new GetVersionReq);
            break;
         case GET_VERSION_REPLY:
            ret.reset(new GetVersionReply);
            break;
      }

      return ret;
   }

   std::vector<uint32_t> getEventIds()
   {
      std::vector<uint32_t> ret;
      ret.push_back(GET_VERSION_REQ);
      ret.push_back(GET_VERSION_REPLY);
      return ret;
   }

   static IMessageFactory& getInstance()
   {
      static TunerMsgFactory factory;
      return factory;
   }
};

// Interface binding

class TunerSkelMessagingImpl
{
   std::auto_ptr<ILocalIfc> m_ifc;

protected:
   virtual std::string getVersion(IfcAddr_t client) = 0;

public:

   inline ILocalIfc *getIfc() const { return m_ifc.get(); }

   void registerInterface(IQueue *queue, const std::string &name = "")
   {
      m_ifc.reset(queue->registerInterface(TunerMsgFactory::getInstance(), name));
   }

   bool dispatch(TsdEvent *msg)
   {
      if (!msg || msg->getReceiverAddr() != m_ifc->getLocalIfcAddr()) {
         return false;
      }

      switch (msg->getEventId()) {
         case GET_VERSION_REQ:
            m_ifc->sendMessage(msg->getSenderAddr(),
               std::auto_ptr<TsdEvent>(new GetVersionReply(
                  getVersion(msg->getSenderAddr()))));
            break;
         default:
            std::cout << "Unknown message on tuner iterface: " << msg->getEventId()
                      << &std::endl;
            break;
      }

      return true;
   }
};

class TunerStubMessagingImpl
   : public ITuner
{
   std::auto_ptr<IQueue> m_queue;
   std::auto_ptr<IRemoteIfc> m_ifc;

public:
   TunerStubMessagingImpl(const std::string &name = "tsd.mib3.radio.tuner")
   {
      m_queue.reset(createQueue("TunerStubMessagingImpl"));
      m_ifc.reset(m_queue->connectInterface(name, TunerMsgFactory::getInstance()));
   }

   TunerStubMessagingImpl(ILocalIfc *provider)
   {
      m_queue.reset(createQueue("TunerStubMessagingImpl"));
      m_ifc.reset(m_queue->connectInterface(provider, TunerMsgFactory::getInstance()));
   }

   ~TunerStubMessagingImpl()
   { }

   std::string getVersion()
   {
      // send message
      m_ifc->sendMessage(std::auto_ptr<TsdEvent>(new GetVersionReq));

      // we expect exactly one message (the reply)
      for (;;) {
         std::auto_ptr<TsdEvent> m = m_queue->readMessage();
         if ((m->getSenderAddr()   == m_ifc->getRemoteIfcAddr()) &&
             (m->getReceiverAddr() == m_ifc->getLocalIfcAddr()) &&
             (m->getEventId()      == GET_VERSION_REPLY)) {

            GetVersionReply *reply = dynamic_cast<GetVersionReply*>(m.get());
            return reply->getVersion();
         } else {
            std::cout << "Unknown reply from tuner iterface: " << m->getEventId()
                      << &std::endl;
         }
      }
   }
};

class TunerStubAsyncMessagingImpl
   : public ITunerAsyncRequest
   , public ITunerAsyncReply
{
   std::auto_ptr<IRemoteIfc> m_ifc;

public:
   virtual ~TunerStubAsyncMessagingImpl()
   { }

   void connectInterface(IQueue *queue, const std::string &name = "tsd.mib3.radio.tuner")
   {
      m_ifc.reset(queue->connectInterface(name, TunerMsgFactory::getInstance()));
   }

   void connectInterface(IQueue *queue, ILocalIfc *provider)
   {
      m_ifc.reset(queue->connectInterface(provider, TunerMsgFactory::getInstance()));
   }

   bool dispatch(TsdEvent *msg)
   {
      if (!msg || msg->getReceiverAddr() != m_ifc->getLocalIfcAddr()) {
         return false;
      }

      switch (msg->getEventId()) {
         case GET_VERSION_REPLY:
         {
            GetVersionReply *reply = dynamic_cast<GetVersionReply*>(msg);
            getVersionReply(reply->getVersion());
            break;
         }
         default:
            std::cout << "Unknown reply from tuner iterface: " << msg->getEventId()
                      << &std::endl;
            break;
      }

      return true;
   }

   // ITunerAsyncRequest implementation

   void getVersionReq()
   {
      m_ifc->sendMessage(std::auto_ptr<TsdEvent>(new GetVersionReq));
   }
};


/******************************************************************************
 * Tuner server implementation
 *****************************************************************************/

class TunerServer
   : public TunerSkelMessagingImpl
{
public:

   std::string getVersion(IfcAddr_t /*client*/)
   {
      return "1.2.3";
   }
};

class Server
   : public tsd::common::system::Thread
{
   tsd::common::system::Semaphore m_startup;
   IQueue *m_queue;

public:
   ILocalIfc *m_tunerIfc;

   Server()
      : tsd::common::system::Thread("server")
      , m_startup(0)
      , m_queue(createQueue("tsd.radio.core"))
      , m_tunerIfc(NULL)
   {
   }

   ~Server()
   {
      if (hasRun()) {
         m_queue->sendSelfMessage(std::auto_ptr<TsdEvent>(new QuitInd));
         join();
      }

      delete m_queue;
   }

   void init()
   {
      start();
      m_startup.down();
   }

   void run()
   {
      TunerServer tunerServer;

      tunerServer.registerInterface(m_queue, "tsd.mib3.radio.tuner");

      m_tunerIfc = tunerServer.getIfc();
      bool alive = true;
      m_startup.up();

      std::cout << "Server waiting for message" << &std::endl;

      while (alive) {
         std::auto_ptr<TsdEvent> msg = m_queue->readMessage();

         // dispatch interface messages
         if (tunerServer.dispatch(msg.get())) {
            continue;
         }

         // provide dispatch class for the following code
         if (msg->getReceiverAddr() == LOOPBACK_ADDRESS) {
            switch (msg->getEventId()) {
               case QUIT_IND:
                  std::cout << "Server got quit message" << &std::endl;
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
};

/******************************************************************************
 * Tuner client implementation
 *****************************************************************************/

class TunerClient
   : public TunerStubAsyncMessagingImpl
{
public:
   void getVersionReply(std::string version)
   {
      std::cout << "Tuner version: " << version
                << &std::endl;
   }
};

static void namedServerSyncTest()
{
   // spawn server process
   Server server;
   server.init();

   TunerStubMessagingImpl tuner;

   std::cout << "Tuner version: " << tuner.getVersion()
             << &std::endl;
}

static void unnamedServerSyncTest()
{
   // spawn server process
   Server server;
   server.init();

   TunerStubMessagingImpl tuner(server.m_tunerIfc);

   std::cout << "Tuner version: " << tuner.getVersion()
             << &std::endl;
}

static void namedServerAsyncTest()
{
   // spawn server process
   Server server;
   server.init();

   // create queue and with client stub
   std::auto_ptr<IQueue> q(createQueue("namedServerAsyncTest"));
   TunerClient tuner;
   tuner.connectInterface(q.get());

   // send request
   tuner.getVersionReq();

   // wait for reply
   for (;;) {
      std::auto_ptr<TsdEvent> msg = q->readMessage();
      if (tuner.dispatch(msg.get())) {
         // done
         break;
      } else {
         std::cout << "Unknown message received" << &std::endl;
      }
   }
}

static void unnamedServerAsyncTest()
{
   // spawn server process
   Server server;
   server.init();

   // create queue and with client stub
   std::auto_ptr<IQueue> q(createQueue("unnamedServerAsyncTest"));
   TunerClient tuner;
   tuner.connectInterface(q.get(), server.m_tunerIfc);

   // send request
   tuner.getVersionReq();

   // wait for reply
   for (;;) {
      std::auto_ptr<TsdEvent> msg = q->readMessage();
      if (tuner.dispatch(msg.get())) {
         // done
         break;
      } else {
         std::cout << "Unknown message received" << &std::endl;
      }
   }
}

int main()
{
   std::cout << ">>> Test named synchronous server..." << &std::endl;
   namedServerSyncTest();

   std::cout << ">>> Test UNNAMED synchronous server..." << &std::endl;
   unnamedServerSyncTest();

   std::cout << ">>> Test named ASYNCHRONOUS server..." << &std::endl;
   namedServerAsyncTest();

   std::cout << ">>> Test UNNAMED ASYNCHRONOUS server..." << &std::endl;
   unnamedServerAsyncTest();

   return 0;
}
