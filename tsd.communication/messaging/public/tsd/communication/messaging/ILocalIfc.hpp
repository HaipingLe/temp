#ifndef TSD_COMMUNICATION_MESSAGING_ILOCALIFC_HPP
#define TSD_COMMUNICATION_MESSAGING_ILOCALIFC_HPP

#include <memory>

#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/types.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Representation of a registered interface.
    *
    * Other participants in the system might connect to this interface and send
    * message to the associated queue. In the other direction this interface can
    * be used to reply or broadcast messages to all connected peers.
    */
   class ILocalIfc {
      tsd::communication::event::IfcAddr_t m_localIfcAddr;

   public:
      ILocalIfc(tsd::communication::event::IfcAddr_t localIfcAddr)
         : m_localIfcAddr(localIfcAddr)
      { }

      /**
       * Destroy interface.
       *
       * The interface is removed from the system and all associated messages
       * are deleted. This includes messages that are still in flight. All
       * monitors installed on this interface are removed too.
       *
       * You have to destroy this and other interfaces before destroying the
       * associated queue.
       */
      virtual ~ILocalIfc();

      /**
       * Send unicast message.
       *
       * Sends a message to a single reciver from this interface. The given
       * destination address must be of a compatible interface, that is some
       * instance that did a connectInterface() for this one here. Otherwise the
       * message cannot be delivered.
       *
       * This method must only be used to reply to addresses that have been
       * received on this interface. Sending messages to unrelated addresses
       * will cause undefined behaviour.
       *
       * @param remoteIfc  Remote interface address where the message is sent to
       * @param msg        The message that is sent
       */
      virtual void sendMessage(tsd::communication::event::IfcAddr_t remoteIfc,
                               std::auto_ptr<tsd::communication::event::TsdEvent> msg) = 0;

      /**
       * Send broadcast message.
       *
       * The message is sent to all remote interfaces that have connected to
       * this local interface.
       *
       * @param msg  The message that is sent to connected interfaces
       */
      virtual void broadcastMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg) = 0;

      /**
       * Start monitoring a remote interface.
       *
       * If the interface disappears the given event will be sent to the
       * implicit loopback interface of the queue. If the interface does not
       * exist when calling the function the message will be sent immediately.
       *
       * Only addresses that have been received on this interface must be
       * monitored. Trying to monitor other addresses is implementation defined
       * and might not work at all.
       *
       * @param event   An event that will be sent to the loopback interface of the queue
       * @param addr    Remote interface address that should be monitored
       * @return Monitor reference that may be used to stop the monitor.
       */
      virtual MonitorRef_t monitor(std::auto_ptr<tsd::communication::event::TsdEvent> event,
                                   tsd::communication::event::IfcAddr_t addr) = 0;

      /**
       * Stop monitoring an interface.
       *
       * It is guaranteed that the monitor message is not received anymore when
       * this method returns.
       *
       * @param monitor Monitor reference that should be deleted
       */
      virtual void demonitor(MonitorRef_t monitor) = 0;

      /**
       * Get local address of this interface.
       *
       * This is where we're receiving the messages.
       *
       * @returns local address
       */
      inline tsd::communication::event::IfcAddr_t getLocalIfcAddr() const
      {
         return m_localIfcAddr;
      }
   };

} } }

#endif
