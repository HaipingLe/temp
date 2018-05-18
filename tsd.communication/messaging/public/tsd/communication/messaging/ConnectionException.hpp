#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTIONEXCEPTION_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTIONEXCEPTION_HPP

#include <tsd/communication/messaging/BaseException.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Exception that is thrown when router connection cannot be established.
    */
   class ConnectionException
      : public BaseException
   {
   public:
      ConnectionException(const std::string &msg)
         : BaseException(msg)
      { }
   };

} } }

#endif

