///////////////////////////////////////////////////////
//!\file Connection.cpp
//!\brief Generic ComMgr connection implementation
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <cstring>
#include <string>

#include <tsd/common/errors/ConnectException.hpp>
#include <tsd/communication/Connection.hpp>
#include <tsd/communication/QnxConnection.hpp>
#include <tsd/communication/TcpConnection.hpp>

using tsd::communication::client::Connection;

tsd::communication::client::IReceiveCallback::~IReceiveCallback()
{
}


Connection::Connection(tsd::communication::client::IReceiveCallback *cb, tsd::common::logging::Logger &log)
   : m_log(log)
   , m_cb(cb)
{
}

Connection::~Connection()
{
}

Connection *Connection::openConnection(tsd::communication::client::IReceiveCallback *cb,
                                       tsd::common::logging::Logger &log,
                                       const char_t *server)
{
   Connection *ret = NULL;
   static const char prefixTcp[] = "tcp://";

   if (server == NULL) {
      server = "LOCAL";
   }
   if (std::strcmp(server, "LOCAL") == 0) {
#ifdef TARGET_OS_POSIX_QNX
      server = "qnx://local/tsd.communication.commgr";
#else
      server = "tcp://";
#endif
   }
   std::string url(server);

#ifdef TARGET_OS_POSIX_QNX
   static const char prefixQnx[]    = "qnx://";
   static const char prefixLocal[]  = "local/";
   static const char prefixGlobal[] = "global/";

   if (url.compare(0, std::strlen(prefixQnx), prefixQnx, std::strlen(prefixQnx)) == 0) {
      url = url.substr(6);
      bool global = false;
      if (url.compare(0, std::strlen(prefixLocal), prefixLocal, std::strlen(prefixLocal)) == 0) {
         url = url.substr(6);
      } else if (url.compare(0, std::strlen(prefixGlobal), prefixGlobal, std::strlen(prefixGlobal)) == 0) {
         global = true;
         url = url.substr(7);
      } else {
         throw tsd::common::errors::ConnectException("Neither local nor global QNX protocol");
      }

      ret = new QnxConnection(cb, log, url.c_str(), global);
   } else
#endif
   if (url.compare(0, std::strlen(prefixTcp), prefixTcp, std::strlen(prefixTcp)) == 0) {
      ret = new TcpConnection(cb, log, url.substr(6));
   } else if (url.find("://") == std::string::npos) {
      // Assume tcp if no schema is specified
      ret = new TcpConnection(cb, log, url);
   } else {
      std::string msg("Invalid protocol: ");
      msg += url;
      throw tsd::common::errors::ConnectException(msg);
   }

   return ret;
}

