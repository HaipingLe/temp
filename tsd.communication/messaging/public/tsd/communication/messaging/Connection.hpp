#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTION_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTION_HPP

#include <string>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Abstract connection class.
    *
    * This class is more like a resource handle to own a client or server
    * connection. The only thing that is possible is to shutdown the
    * connection. Destroying the object will free all underlying resources.
    */
   class IConnection
   {
   public:
      virtual ~IConnection();

      /**
       * Shutdown connection.
       */
      virtual void disconnect() = 0;
   };

   class ConnectionImpl;

   class IConnectionCallbacks
   {
   public:
      virtual ~IConnectionCallbacks();

      virtual bool setupConnection() = 0;
      virtual void teardownConnection() = 0;
      virtual void wakeup() = 0;
   };

   /**
    * Abstract point-to-point transport class.
    *
    * To implement special purpose transports derive from this class. It
    * assumes a reliable byte stream that does not preserve packet bounderies.
    * The deriving class is expected to use a dedicated io-thread for the
    * actual byte transport.
    */
   class Connection
      : protected IConnectionCallbacks
      , public IConnection
   {
      ConnectionImpl *m_p;

   protected:
      Connection();
      virtual ~Connection();

      void setConnected();
      void setDisconnected();

      void processData(const void* data, size_t size);
      bool getData(const void*& data, size_t& size);
      void consumeData(size_t amount);

   public:
      bool connectUpstream(const std::string &subDomain = "");
      bool listenDownstream();
      void disconnect();
   };

   /**
    * Connect to an upstream router through a standard protocol.
    *
    * Depending on the platform a suitable default upstream router location
    * will be choosen. If you want to connect to a specific router specify
    * its address in @p url.
    *
    * By default the local router will join the domain of the upstream router.
    * To allocate a private sub-domain set @p subDomain to the requested domain
    * name.
    *
    * Currently supported protocols:
    *  * TCP: "tcp://[x.x.x.x[:port]]"
    *  * UIO: "uio:///dev/uioX"
    *
    * @throw ConnectionException Connection could not be esablished
    *
    * @return A pointer to an abstract connection. The caller is responsible to
    *         delete the connection object.
    */
   IConnection *connectUpstream(const std::string &url = "",
                                const std::string &subDomain = "");

   /**
    * Listen for downstream router(s) to connect through a standard protocol.
    *
    * Currently supported protocols:
    *  * TCP: "tcp://[x.x.x.x[:port]]"
    *  * UIO: "uio:///dev/uioX"
    *
    * @throw ConnectionException Connection could not be esablished
    *
    * @return A pointer to an abstract connection. The caller is responsible to
    *         delete the connection object.
    */
   IConnection *listenDownstream(const std::string &url);

} } }

#endif

