#ifndef TSD_COMMUNICATION_MESSAGING_IREMOTEIFC_HPP
#define TSD_COMMUNICATION_MESSAGING_IREMOTEIFC_HPP

#include <memory>
#include <vector>

#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/types.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Representation of a connected interface.
    *
    */
   class IRemoteIfc {
      tsd::communication::event::IfcAddr_t m_localIfcAddr;
      tsd::communication::event::IfcAddr_t m_remoteIfcAddr;

   public:
      IRemoteIfc(tsd::communication::event::IfcAddr_t localIfcAddr,
                 tsd::communication::event::IfcAddr_t remoteIfcAddr)
         : m_localIfcAddr(localIfcAddr)
         , m_remoteIfcAddr(remoteIfcAddr)
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
      virtual ~IRemoteIfc();

      /**
       * Send message to service.
       *
       * The message is sent to the remote interface that was looked up by
       * lookupInterface().
       *
       * @param msg The message that is sent
       */
      virtual void sendMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg) = 0;

      /**
       * Start monitoring the remote interface.
       *
       * If the interface disappears the given event will be sent to the
       * implicit loopback interface of the queue. If the interface does not
       * exist when calling the function the message will be sent immediately.
       *
       * @param event   An event that will be sent to the loopback interface of the queue
       * @return Monitor reference that may be used to stop the monitor.
       */
      virtual MonitorRef_t monitor(std::auto_ptr<tsd::communication::event::TsdEvent> event) = 0;

      /**
       * Stop monitoring an interface.
       *
       * It is guaranteed that no monitor message will be received after the
       * call returned. If the corresponding message is already in the queue it
       * will be dropped.
       *
       * @param monitor Monitor reference that should be deleted
       */
      virtual void demonitor(MonitorRef_t monitor) = 0;

      /**
       * Subscribe to broadcast event.
       *
       * Only events that have been subscribed are received from server
       * broadcasts. Subscribing multiple times to the same event is allowed
       * but the subscriptions are not reference counted. A single
       * unsubscribe() is sufficient to remove the subscription.
       *
       * @param event The event that should be received
       */
      virtual void subscribe(uint32_t event) = 0;

      /**
       * Subscribe to broadcast events.
       *
       * Only events that have been subscribed are received from server
       * broadcasts. Subscribing multiple times to the same event is allowed
       * but the subscriptions are not reference counted. A single
       * unsubscribe() is sufficient to remove the subscription.
       *
       * @param events The events that should be received
       */
      virtual void subscribe(const std::vector<uint32_t> &events) = 0;

      /**
       * Unsubscribe from broadcast event.
       *
       * Remove the subscription made for a broadcast event via subscribe(). It
       * is allowed to unsubscribe() from an event that is not currently
       * subscribed. Be aware that the unsubscribed event might already be in
       * the queue and thus can be received even after this method has
       * returned.
       *
       * @param event The event that shall not be received anymore
       */
      virtual void unsubscribe(uint32_t event) = 0;

      /**
       * Unsubscribe from broadcast events.
       *
       * Remove the subscriptions made for a number of broadcast events via
       * subscribe(). It is allowed to unsubscribe() from events that are not
       * currently subscribed. Be aware that any of the unsubscribed events
       * might already be in the queue and thus can be received even after this
       * method has returned.
       *
       * @param event The event that shall not be received anymore
       */
      virtual void unsubscribe(const std::vector<uint32_t> &events) = 0;

      /**
       * Get our local address here.
       *
       * This is where we're receiving the messages.
       *
       * @return local address
       */
      inline tsd::communication::event::IfcAddr_t getLocalIfcAddr() const
      {
         return m_localIfcAddr;
      }

      /**
       * Get the remote address of the interface provider.
       *
       * This is the address of the interface where the messages will be sent
       * to.
       *
       * @return remote address
       */
      inline tsd::communication::event::IfcAddr_t getRemoteIfcAddr() const
      {
         return m_remoteIfcAddr;
      }
   };

} } }

#endif
