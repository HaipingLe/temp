/**
 * \file SendReceiveBackend.cpp
 * \brief QNX MsgSend/MsgReceive backend implementation
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */

#include <queue>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include <sstream>

#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/communication/SendReceiveBackend.hpp>
#include <tsd/communication/Client.hpp>
#include <tsd/communication/Buffer.hpp>
#include <tsd/common/errors/ConnectException.hpp>


#define CM_PULSE_STOP (_PULSE_CODE_MINAVAIL)

#define CM_RECEIVE_MESSAGE    0
#define CM_SEND_MESSAGE       1
#define CM_CONNECT_MESSAGE    2

namespace {
   inline uint64_t makeClientId(int32_t scoid, int32_t coid)
   {
      return (static_cast<uint64_t>(scoid) << 32) | static_cast<uint32_t>(coid);
   }

   inline uint64_t minClientId(int32_t scoid)
   {
      return (static_cast<uint64_t>(scoid) << 32);
   }

   inline uint64_t maxClientId(int32_t scoid)
   {
      return (static_cast<uint64_t>(scoid) << 32) | UINT32_MAX;
   }
}

namespace tsd { namespace communication {

   struct CMMessage {
      uint16_t type;
      uint16_t _unused;
      uint32_t length;
      uint32_t payload[30];
   };

   union AllMessages {
      uint16_t type;
      struct _pulse pulse;
      CMMessage msg;
   };

   class SendReceiveClient : public Client
   {
      public:
         SendReceiveClient() : m_rcvid(0), m_rcvlen(0) { }

         void unblockClient();
         void receiveMessage(int rcvid, CMMessage *msg);

         virtual void pushMessage(Buffer *msg);

      private:
         void receiveMessageReply(int rcvid, uint32_t rcvlen);

         int m_rcvid;
         uint32_t m_rcvlen;
         std::queue<Buffer*> m_queue;
   };
}}

using tsd::communication::SendReceiveBackend;
using tsd::communication::SendReceiveClient;
using tsd::communication::CommunicationManager;


SendReceiveBackend::SendReceiveBackend(CommunicationManager &cm,
                                       tsd::common::logging::Logger &logger,
                                       const std::string &address,
                                       bool global)
   : Thread("tsd.communication.commgr.qnx"),
     m_cm(cm),
     m_log(logger)
{
   m_attach = name_attach(NULL, address.c_str(), global ? NAME_FLAG_ATTACH_GLOBAL : 0);
   if (!m_attach) {
      std::stringstream s;
      s << "name_attach failed w/ " << errno;
      throw tsd::common::errors::ConnectException(s.str());
   }

   start();
}

SendReceiveBackend::~SendReceiveBackend()
{
   int coid = ConnectAttach_r(ND_LOCAL_NODE, 0, m_attach->chid, _NTO_SIDE_CHANNEL, 0);
   MsgSendPulse(coid, -1, CM_PULSE_STOP, 0);
   join();
   ConnectDetach(coid);

   name_detach(m_attach, 0);
}

void SendReceiveBackend::run()
{
   AllMessages msg;
   struct _msg_info info;
   bool run = true;

   while (run) {
      int rcvid = MsgReceive(m_attach->chid, &msg, sizeof(msg), &info);

//      m_log << tsd::common::logging::LogLevel::Trace << "MsgReceive id:" << rcvid
//            << " type:" << msg.type << std::endl;

      m_cm.lock();

      if (rcvid == -1) {
         int err = errno;
         if (err != EINTR) {
            m_log << tsd::common::logging::LogLevel::Error << "MsgReceive " << err
                  << std::endl;
            run = false;
         }
      } else if (rcvid == 0) {
         if (handlePulse(&msg.pulse))
            run = false;
      } else {
         handleMessage(rcvid, &msg.msg, &info);
      }

      m_cm.unlock();
   }
}

bool SendReceiveBackend::handlePulse(struct _pulse *pulse)
{
   bool ret = false;

   switch (pulse->code) {
      case _PULSE_CODE_DISCONNECT:
         /*
          * A client disconnected all its connections (called
          * name_close() for each name_open() of our name) or
          * terminated
          */
         ConnectDetach(pulse->scoid);
         deregisterClients(pulse->scoid);
         break;

      case _PULSE_CODE_UNBLOCK:
      {
         /*
          * REPLY blocked client wants to unblock (was hit by a signal or timed
          * out). Get corresponding client id from affected rcvid and unblock
          * client.
          */
         struct _msg_info info;
         if (MsgInfo_r(pulse->value.sival_int, &info) == EOK) {
            m_clients[makeClientId(info.scoid, info.coid)]->unblockClient();
         }
         break;
      }

      case CM_PULSE_STOP:
         ret = true;
         break;

      default:
         /*
          * A pulse sent by one of your processes or a
          * _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
          * from the kernel?
          */
         break;
   }

   return ret;
}

void SendReceiveBackend::handleMessage(int rcvid, CMMessage *msg, struct _msg_info *info)
{
   switch (msg->type) {
      case CM_RECEIVE_MESSAGE:
         m_clients[makeClientId(info->scoid, info->coid)]->receiveMessage(rcvid, msg);
         break;
      case CM_SEND_MESSAGE:
         clientSendMessage(rcvid, msg, m_clients[makeClientId(info->scoid, info->coid)]);
         break;
      case _IO_CONNECT:
      case CM_CONNECT_MESSAGE:
         /* name_open() sends a connect message, must EOK this */
         registerClient(info);
         MsgReply(rcvid, EOK, NULL, 0);
         break;
      default:
         /* Some other QNX IO message was received; reject it */
         MsgError(rcvid, ENOSYS);
         break;
   }
}

void SendReceiveBackend::registerClient(struct _msg_info *info)
{
   uint64_t clientId = makeClientId(info->scoid, info->coid);

//   m_log << tsd::common::logging::LogLevel::Trace << "registerClient " << info->scoid
//         << ":" << info->coid << std::endl;

   if (m_clients.find(clientId) != m_clients.end())
      return; // already known

   SendReceiveClient *client = new SendReceiveClient;
   client->pid = info->pid;
   m_cm.registerClient(client);
   m_clients[clientId] = client;
}

void SendReceiveBackend::deregisterClients(int32_t scoid)
{
//   m_log << tsd::common::logging::LogLevel::Trace << "deregisterClients " << scoid
//         << std::endl;

   ClientMap::iterator it(m_clients.lower_bound(minClientId(scoid)));
   uint64_t lastClientId = maxClientId(scoid);
   while (it != m_clients.end() && it->first <= lastClientId) {
      SendReceiveClient *client = it->second;
      m_cm.deregisterClient(client);
      delete client;

      // advance iterator _before_ deleting the element
      ClientMap::iterator delIt(it++);
      m_clients.erase(delIt);
   }
}

void SendReceiveBackend::clientSendMessage(int rcvid, CMMessage *msg, SendReceiveClient *client)
{
   Buffer *buf = allocBuffer(msg->length);
   if (msg->length > sizeof(msg->payload)) {
      MsgRead(rcvid, buf->payload(), msg->length, 8);
   } else {
      std::memcpy(buf->payload(), msg->payload, msg->length);
   }
   MsgReply(rcvid, EOK, NULL, 0);

   m_cm.dispatchMessage(client, buf);
}



void SendReceiveClient::unblockClient()
{
   // unblock client if it tries to receive a message
   if (m_rcvid != 0) {
      MsgReply(m_rcvid, EINTR, NULL, 0);
      m_rcvid = 0;
   }
}

void SendReceiveClient::receiveMessage(int rcvid, CMMessage *msg)
{
   if (m_rcvid != 0) {
      MsgError(rcvid, EBUSY);
   } else {
      if (m_queue.empty()) {
         m_rcvid = rcvid;
         m_rcvlen = msg->length;
      } else {
         receiveMessageReply(rcvid, msg->length);
      }
   }
}

void SendReceiveClient::receiveMessageReply(int rcvid, uint32_t rcvlen)
{
   if (!m_queue.empty())
   {
      Buffer *msg = m_queue.front();
      uint32_t hdr = msg->length();

      if ((msg->length() + sizeof(hdr)) <= rcvlen) {
         iov_t iov[2];
         iov[0].iov_base = &hdr;
         iov[0].iov_len = sizeof(hdr);
         iov[1].iov_base = msg->payload();
         iov[1].iov_len = msg->length();
         if (MsgReplyv(rcvid, EOK, iov, 2) != -1) {
            m_queue.pop();
            msg->deref();
         }
      } else {
         MsgReply(rcvid, hdr + sizeof(hdr), NULL, 0);
      }
   }
}

void SendReceiveClient::pushMessage(Buffer *msg)
{
   msg->ref();
   m_queue.push(msg);

   if (m_rcvid) {
      receiveMessageReply(m_rcvid, m_rcvlen);
      m_rcvid = 0;
   }
}

