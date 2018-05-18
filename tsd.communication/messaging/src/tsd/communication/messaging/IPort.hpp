#ifndef TSD_COMMUNICATION_MESSAGING_IPORT_HPP
#define TSD_COMMUNICATION_MESSAGING_IPORT_HPP

#include <memory>

#include <tsd/common/system/CondVar.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/common/system/MutexGuard.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/utils.hpp>

namespace tsd { namespace communication { namespace messaging {

class Packet;
class Router;

/**
 * Port base class that connects two routers.
 *
 * The current router implementation impies a tree topology. This is reflected
 * on the ports where a port is either directed upstream or downstream wrt. a
 * router.
 *
 * The IPort class has some subtle threading requirements that must be obeyed.
 * Normally two threads are involved in the port life cycle: the user thread
 * that is setting up the connection (later called main thread) and the
 * IO-thread of the port that moves the data through the port. The main thread
 * is expected to call initDownstream() or initUpstream(). This in turn will
 * call startPort() which is expected to spawn the io-thread. Only the
 * io-thread must call connected() or disconnected() to set the connection
 * state.  The main thread may call finish() at any time to tear down the port.
 * This will invoke stopPort() that must only return after the io-thread was
 * reaped. The sub-class may still receive sendPacket() calls until the
 * finish() method has returned to the main thread.
 *
 * The sendPacket() method may be called by any thread and must synchronize
 * itself as necessary.
 */
class IPort
{
   enum State {
      UNINITIALIZED,
      UPSTREAM_UNBOUND,
      UPSTREAM_BOUND,
      UPSTREAM_CONNECTED,
      UPSTREAM_DEAD,
      DOWNSTREAM_UNCONNECTED,
      DOWNSTREAM_CONNECTED,
   };

   Router &m_router;
   tsd::communication::event::IfcAddr_t m_localAddr;
   tsd::communication::event::IfcAddr_t m_peerAddr;
   tsd::communication::event::IfcAddr_t m_mask;
   uint8_t m_prefixLength;
   State m_state;
   bool m_upstream;
   tsd::common::system::Mutex m_lock;
   tsd::common::system::CondVar m_stateCondition;
   tsd::communication::event::IfcAddr_t m_nameServer;
   thread_id_t m_ioThreadId;

   void processUnboundPacket(std::auto_ptr<Packet> pkt);

protected:
   /**
    * Pass received packet to router.
    *
    * Called by the io-thread of the port to pass a received packet into the
    * communication stack.
    *
    * @param pkt  The received packet
    */
   void receivedPacket(std::auto_ptr<Packet> pkt);

   /**
    * Signal that connection is up.
    *
    * Called by the io-thread to signal that the connection to the other router
    * is established and that packets can be sent to the other side. This will
    * allocate some resources for the connection. If the allocation fails the
    * method will return "false". In this case the connection should be dropped
    * because it is dysfunctional.
    *
    * @return True if connection resources could be alllocated, false otherwise.
    */
   bool connected();

   /**
    * Signal that connection is broken.
    *
    * Called by the io-thread to signal that there is no connection to the
    * other router.  All packets will be dropped.
    */
   void disconnected();

   /**
    * Callback to start the io-thread.
    *
    * The port implementation should spawn the io-thread and establish the
    * connection.
    *
    * @return True if port could be initialized, false otherwise.
    */
   virtual bool startPort() = 0;

   /**
    * Callback to reap the io-thread.
    *
    * The port implementation should stop the io-thread and wait until it has
    * stopped.  This method must ensure that neither connected() nor
    * disconnected() are called after it has returned.
    */
   virtual void stopPort() = 0;

public:
   IPort(Router &router);
   virtual ~IPort();

   bool initDownstream();
   bool initUpstream(const std::string &subDomain);
   void finish();

   /**
    * Callback to send packets.
    *
    * This method can be called from any thread and should implement whatever
    * synchronization is necessary for itself.
    *
    * @return True if the packet was sent to the other router. This does not
    *         imply that the other side actually received the packet.
    */
   virtual bool sendPacket(std::auto_ptr<Packet> pkt) = 0;

   inline Router& getRouter() const { return m_router; }

   inline tsd::communication::event::IfcAddr_t getLocalAddr()
   {
      tsd::common::system::MutexGuard g(m_lock);
      return m_localAddr;
   }

   inline tsd::communication::event::IfcAddr_t getPeerAddr()
   {
      tsd::common::system::MutexGuard g(m_lock);
      return m_peerAddr;
   }

   inline tsd::communication::event::IfcAddr_t getMask()
   {
      tsd::common::system::MutexGuard g(m_lock);
      return m_mask;
   }

   inline uint8_t getPrefixLength()
   {
      tsd::common::system::MutexGuard g(m_lock);
      return m_prefixLength;
   }

   inline bool isUpstream() const { return m_upstream; }
   inline bool isDownstream() const { return !m_upstream; }

   inline bool isPortAddr(tsd::communication::event::IfcAddr_t addr) const
   {
      return isHostAddr(addr, m_localAddr);
   }

   inline bool isPortNet(tsd::communication::event::IfcAddr_t addr) const
   {
      return isNetworkAddr(addr, m_localAddr, m_mask);
   }
};

} } }

#endif
