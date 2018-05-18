///////////////////////////////////////////////////////
//!\file QnxConnection.hpp
//!\brief Declaraion of the QNX MsgSend/MsgReceive connection
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_QNXCONNECTION_HPP
#define TSD_COMMUNICATION_QNXCONNECTION_HPP

#include <tsd/common/types/typedef.hpp>
#include <tsd/common/system/Thread.hpp>
#include <tsd/communication/Connection.hpp>

namespace tsd { namespace communication { namespace client {

enum QnxMessageCodes {
   CMReceiveMessage = 0,
   CMSendMessage    = 1,
   CMConnectMessage = 2
};

class QnxConnection : public Connection,
                      public tsd::common::system::Thread
{
public:
   QnxConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log,
                 const char *name, bool global);
   virtual ~QnxConnection();

   virtual bool send(const void *buf, uint32_t len);
   virtual bool send(Buffer *buf);

protected:
   virtual void run(void);

private:
   int m_coid;
   bool m_running;

   uint32_t *m_receiveBuffer;
   uint32_t m_receiveBufferLen;
};

}}}

#endif
