#ifndef TSD_COMMUNICATION_MESSAGING_IQUEUE_HPP
#define TSD_COMMUNICATION_MESSAGING_IQUEUE_HPP

#include <memory>
#include <vector>

#include <tsd/common/types/typedef.hpp>

#include <tsd/communication/event/TsdEvent.hpp>
#include <tsd/communication/messaging/ILocalIfc.hpp>
#include <tsd/communication/messaging/IRemoteIfc.hpp>
#include <tsd/communication/messaging/types.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Factory interface for messages.
    *
    * Used to create message instances for given message IDs. This interface is
    * also used for introspection to query the supported message IDs of the
    * registered/connected interface.
    */
   class IMessageFactory {
   public:
      /**
       * Message factory.
       *
       * @param  msgId    Message-Id of the event
       * @return New subclass instance for the given message-Id.
       */
      virtual std::auto_ptr<tsd::communication::event::TsdEvent>
      createEvent(uint32_t msgId) const = 0;

      /**
       * Return list of supported message IDs.
       *
       * This is used for debugging to introspect the system. This is not
       * called under normal circumstances. The default implementation
       * returns nothing.
       *
       * @return vector of supported message IDs
       */
      virtual std::vector<uint32_t> getEventIds() const;
   };

   /**
    * Utility class to filter received messages.
    *
    * This interface can be used to implement a filter to receive a subset of
    * all messages that might hit a queue. This is typically used if the caller
    * waits for a specific reply. Instead of having to handle all possible
    * messages the caller restricts it to a subset that is expected to arrive.
    */
   class IMessageSelector {
   public:
      /**
       * Selector function for IQueue::readMessage().
       *
       * The method must not modify any external state. It is unspecified in
       * which context and how often the method is called.
       *
       * @param event  The event that should be checked
       * @return True if the message should be received.
       */
      virtual bool filterEvent(tsd::communication::event::TsdEvent *event) const = 0;
   };

   /**
    * Queue that is storing received messages.
    *
    * The queue is a FIFO storage area. Messages are stored only at the
    * destination queue.
    *
    * Messages are transported in-order on a best-effort basis through the
    * system. Delivery is not guaranteed but the following restrictions apply:
    * 1. The system guarantees the message ordering between two queues.
    * 2. If a transmitter sends two message A and B (in this order) and the
    *    receiver got message B it is guaranteed that the receiver got message
    *    A before.
    *
    * It is possible for multiple threads to receive messages from a shared
    * queue. Each message is only delivered to a single thread. Which thread
    * receives the message is unspecified if multiple threads are blocked in
    * readMessage() simultaneously.
    */
   class IQueue {
   public:
      /**
       * Destroy the queue.
       *
       * It is the callers responsibility to destroy all registered or
       * connected interfaces before actually destroying the queue.
       */
      virtual ~IQueue();

      /**
       * Register message interface, optionally under a unique name in the
       * global name space.
       *
       * If the name has already been claimed by someone else the function will
       * throw an AddressInUseException. Upon return the interface is visible
       * for others. If registered globally the message factory is used to
       * create the TsdEvent message instances for received messages that need
       * to be de-serialized. Otherwise the factory will only be used to query
       * debug information.
       *
       * The name for the interface might be subject to implementation defined
       * restrictions. For example the string might need to have a certain
       * prefix based on the deployment location (e.g. "net." vs. "app."). Such
       * restrictions shall be documented by the actual implementation and
       * portable applications should not hard-code the interface name.
       *
       * To de-register the interface just delete the returned ILocalIfc. Any
       * messages that are still in flight or are already in the queue for this
       * interface are dumped silently.
       *
       * @param factory       Message factory for TsdEvent's of this interface
       * @param interfaceName Optional (globally unique) interface name
       *
       * @return Stub of local interface
       *
       * @throw AddressInUseException  Interface name was already taken
       */
      virtual ILocalIfc *registerInterface(const IMessageFactory &factory,
                                           const std::string &interfaceName = "") = 0;

      /**
       * Connect to interfaces on other message queues by name.
       *
       * The other interface has to be registered on a globally visible message
       * queue with the given name. The message factory is used to create the
       * TsdEvent message instances for received messages that need to be
       * de-serialized.
       *
       * By default the method will wait indefinitely until the requested
       * interface is registered in the system. To wait only for a certain time
       * specify the @p timeout parameter accordingly. It is implementation
       * defined whether a timeout of zero milliseconds does anything useful.
       * If the timeout passes before the interface showed up the function will
       * return NULL.
       *
       * To deregister the interface just delete the returned ILocalIfc. Any
       * messages that are still in flight or are already in the queue for this
       * interface are dumped silently.
       *
       * @param interfaceName Global name of the remote interface
       * @param factory       Message factory for TsdEvent's of this interface
       * @param timeout       Timeout in ms to wait for the interface
       *
       * @return Stub of local interface or NULL if a timeout happened
       */
      virtual IRemoteIfc *connectInterface(const std::string &interfaceName,
                                           const IMessageFactory &factory,
                                           uint32_t timeout = INFINITE_TIMEOUT) = 0;

      /**
       * Connect to interface of other queue in local address space.
       *
       * The function fails with an InvalidArgumentException if the local
       * interface is from this queue or from an incompatible implementation.
       * The factory will only be used to query debug information as no
       * de-serialization is needed when communicating in the local address
       * space.
       *
       * @param localIfc   Local interface stub from another queue
       * @param factory    Message factory for debug queries
       *
       * @return Stub of local interface
       *
       * @throw InvalidArgumentException Unsupported interface supplied
       */
      virtual IRemoteIfc *connectInterface(const ILocalIfc *localIfc,
                                           const IMessageFactory &factory) = 0;

      /**
       * Pull next message from the queue.
       *
       * The function blocks until a new message is in the queue or the timeout
       * is reached. Multiple threads may call this function to share the work.
       * If a @p selector is given only messages that pass the selector are
       * returned. Any other messages are left in the queue.
       *
       * @param timeout Timeout in ms to wait for new messages
       * @param selector Selector to restrict messages to a certain subset
       * @return Event from the queue or NULL if timeout passed before
       */
      virtual std::auto_ptr<tsd::communication::event::TsdEvent>
      readMessage(uint32_t timeout = INFINITE_TIMEOUT,
                  IMessageSelector *selector = NULL) = 0;

      /**
       * Send message to the loopback interface of this queue.
       *
       * The message receiving address will be LOOPBACK_ADDRESS. The sending
       * address of @p msg is not touched.
       *
       * @param msg  Message that should be added at the tail of the queue
       */
      virtual void sendSelfMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg) = 0;

      /**
       * Send message after timeout.
       *
       * The message will be delivered to the loopback interface
       * (LOOPBACK_ADDRESS) of the queue after the timer expired. The sending
       * address of the supplied message is not touched.
       *
       * @param event   Message that is sent after the timer has fired
       * @param ms      Timer value in milliseconds
       * @param cyclic  True for a cyclic timer, false for a one-shot timer.
       * @return Handle representing the timer
       */
      virtual TimerRef_t startTimer(std::auto_ptr<tsd::communication::event::TsdEvent> event,
                                    uint32_t ms, bool cyclic) = 0;

      /**
       * Stop a running timer.
       *
       * The timer message will not be delivered anymore after the call has
       * returned. If the timer message was already in the queue the function
       * will return true and the message will be removed from the queue.
       *
       * @param timer   Timer handle as returned by startTimer()
       * @return True if the timer was still pending.
       */
      virtual bool stopTimer(TimerRef_t timer) = 0;
   };

} } }

#endif
