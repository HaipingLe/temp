#ifndef TSD_COMMUNICATION_MESSAGEING_BASEEXCEPTION_HPP
#define TSD_COMMUNICATION_MESSAGEING_BASEEXCEPTION_HPP

#include <exception>
#include <string>

namespace tsd { namespace communication { namespace messaging {

   /**
    * Base class for ComClient exceptions.
    */
   class BaseException
      : public std::exception
   {
      std::string m_what;

   public:
      BaseException(const std::string &msg)
         : m_what(msg)
      { }

      ~BaseException() throw()
      { }

      /**
       * Return string describing the exact reason.
       *
       * @return Exception reason
       */
      const char* what() const throw()
      {
         return m_what.c_str();
      }
   };

} } }

#endif
