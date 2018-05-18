#ifndef TSD_COMMUNICATION_MESSAGING_ADDRESSINUSEEXCEPTION_HPP
#define TSD_COMMUNICATION_MESSAGING_ADDRESSINUSEEXCEPTION_HPP

#include <tsd/communication/messaging/BaseException.hpp>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Exception that is thrown when a name is already in use in the system.
    */
   class AddressInUseException
      : public BaseException
   {
   public:
      AddressInUseException(const std::string &msg)
         : BaseException(msg)
      { }
   };

} } }

#endif
