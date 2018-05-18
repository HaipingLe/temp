#ifndef TSD_COMMUNICATION_MESSAGING_INVALIDARGUMENTEXCEPTION_HPP
#define TSD_COMMUNICATION_MESSAGING_INVALIDARGUMENTEXCEPTION_HPP

#include <tsd/communication/messaging/BaseException.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Exception that is thrown when the API is called with invalid arguments.
    */
   class InvalidArgumentException
      : public BaseException
   {
   public:
      InvalidArgumentException(const std::string &msg)
         : BaseException(msg)
      { }
   };

} } }

#endif
