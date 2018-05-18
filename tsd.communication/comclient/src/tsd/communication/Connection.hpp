///////////////////////////////////////////////////////
//!\file Connection.hpp
//!\brief Generic ComMgr connection definition
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_CONNECTION_HPP
#define TSD_COMMUNICATION_CONNECTION_HPP

#include <tsd/common/types/typedef.hpp>

namespace tsd {
   namespace communication {
      struct Buffer;
   }
   namespace common { namespace logging {
      class Logger;
   } }
}


namespace tsd { namespace communication { namespace client {


class IReceiveCallback
{
public:
   virtual ~IReceiveCallback();

   virtual void messageReceived(const void *buf, uint32_t len) = 0;
   virtual void disconnected() = 0;
};


class TSD_COMMUNICATION_COMCLIENT_DLLEXPORT Connection
{
public:
   virtual ~Connection();

   /*
    * default server (NULL ptr or "LOCAL"): qnx://local/tsd.communication.commgr
    *
    * ip:  tcp://
    *      tcp://x.x.x.x
    *      tcp://x.x.x.x:port
    *
    * qnx: qnx://local/name
    *      qnx://global/name
    */
   static Connection *openConnection(IReceiveCallback *cb,
                                     tsd::common::logging::Logger &log,
                                     const char_t *server);

   virtual bool send(const void *buf, uint32_t len) = 0;
   virtual bool send(Buffer *buf) = 0;

protected:
   Connection(IReceiveCallback *cb, tsd::common::logging::Logger &log);

   inline void received(const void *buf, uint32_t len)
   {
      m_cb->messageReceived(buf, len);
   }

   inline void disconnected()
   {
      m_cb->disconnected();
   }

   tsd::common::logging::Logger &m_log;

private:
   IReceiveCallback *m_cb;
};

} /* namespace client */ } /* namespace communication */ } /* namespace tsd */

#endif // TSD_COMMUNICATION_CONNECTION_HPP
