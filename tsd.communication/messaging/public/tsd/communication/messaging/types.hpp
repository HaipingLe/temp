#ifndef TSD_COMMUNICATION_MESSAGING_TYPES_HPP
#define TSD_COMMUNICATION_MESSAGING_TYPES_HPP

#include <tsd/common/types/typedef.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Opaque handle for monitors.
    */
   typedef uint32_t MonitorRef_t;

   /**
    * Opaque handle for timers.
    */
   typedef uint32_t TimerRef_t;

   /**
    * Constant representing an infinite timeout.
    */
   static const uint32_t INFINITE_TIMEOUT = uint32_t(-1);

} } }

#endif
