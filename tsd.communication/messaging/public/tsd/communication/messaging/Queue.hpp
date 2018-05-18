#ifndef TSD_COMMUNICATION_MESSAGING_QUEUE_HPP
#define TSD_COMMUNICATION_MESSAGING_QUEUE_HPP

#include <tsd/communication/messaging/IQueue.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Create message queue.
    *
    * The name is purely used for debugging.  Please supply a recognizable name
    * because this is what will be visible if the watchdog bites the stuck
    * queue.
    */
   IQueue *createQueue(const std::string &name);

} } }

#endif
