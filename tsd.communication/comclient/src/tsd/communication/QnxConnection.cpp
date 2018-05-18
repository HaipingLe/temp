///////////////////////////////////////////////////////
//!\file QnxConnection.cpp
//!\brief Implementation of the QNX MsgSend/MsgReceive connection
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <errno.h>

#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/QnxConnection.hpp>
#include <tsd/common/errors/ConnectException.hpp>
#include <tsd/common/logging/Logger.hpp>

using tsd::communication::client::QnxConnection;

typedef union {
   struct {
      uint16_t type;
      uint16_t subtype;
   } hdr;

   struct {
      uint16_t type;
      uint16_t unused;
      uint32_t version;
   } initReq;

   struct {
     uint16_t type;
     uint16_t unused;
     uint32_t length; // without header
   } sndRcvReq;
} ComMgrMessage;


QnxConnection::QnxConnection(IReceiveCallback *cb, tsd::common::logging::Logger &log,
                             const char *name, bool global)
   : Connection(cb, log)
   , tsd::common::system::Thread("tsd.communication.comclient.qnx")
{
   m_coid = name_open(name, global ? NAME_FLAG_ATTACH_GLOBAL : 0);
   if (m_coid == -1) {
      throw tsd::common::errors::ConnectException("Could not connect to QNX ComMgr");
   }

   ComMgrMessage initMsg;
   initMsg.initReq.type = CMConnectMessage;
   initMsg.initReq.version = 0;
   int ret = MsgSend(m_coid, &initMsg, sizeof(initMsg), NULL, 0);
   if (ret == -1) {
      throw tsd::common::errors::ConnectException("InitReq failed");
   }

   m_receiveBufferLen = 32768;
   m_receiveBuffer = new uint32_t[m_receiveBufferLen / sizeof(uint32_t)];

   m_running = true;
   start();
}

QnxConnection::~QnxConnection()
{
   m_running = false;
   name_close(m_coid); // will cause MsgSend to return
   join();
   delete [] m_receiveBuffer;
}

void QnxConnection::run()
{
   while (m_running) {
      ComMgrMessage getReq;
      getReq.sndRcvReq.type = CMReceiveMessage;
      getReq.sndRcvReq.length = m_receiveBufferLen;

      int ret = MsgSend_r(m_coid, &getReq, sizeof(getReq), m_receiveBuffer, m_receiveBufferLen);
      if (ret == 0) {
         // successfully received a message
         uint32_t len = m_receiveBuffer[0];
         received(m_receiveBuffer + 1, len);
      } else if (ret < 0) {
         // something went wrong, just EINTR would be ok
         if (ret != -EINTR) {
            if (m_running) {
               m_log << tsd::common::logging::LogLevel::Warn
                     << "QnxConnection::run(): MsgSend failed " << ret << std::endl;
            }
            break; // quit thread
         }
      } else {
         // our buffer was too small; retry...
         delete [] m_receiveBuffer;
         m_receiveBufferLen = ret;
         m_receiveBuffer = new uint32_t[(ret + sizeof(uint32_t) - 1) / sizeof(uint32_t)];
      }
   }
}

bool QnxConnection::send(const void *buf, uint32_t len)
{
   ComMgrMessage sendReq;
   sendReq.sndRcvReq.type = CMSendMessage;
   sendReq.sndRcvReq.length = len;

   iovec outputData[2];
   outputData[0].iov_base = &sendReq;
   outputData[0].iov_len = sizeof(sendReq.sndRcvReq);
   outputData[1].iov_base = const_cast<void*>(buf);
   outputData[1].iov_len = len;

   return MsgSendv(m_coid, outputData, 2, NULL, 0) == 0;
}

bool QnxConnection::send(Buffer *buf)
{
   return send(buf->payload(), buf->length());
}

