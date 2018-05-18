/**
 * \file SendReceiveBackend.hpp
 * \brief QNX MsgSend/MsgReceive backend definition
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */
#ifndef __SENDRECEIVEBACKEND_HPP_
#define __SENDRECEIVEBACKEND_HPP_

#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <map>

#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/common/system/Thread.hpp>

struct _pulse;

namespace tsd { namespace communication {

struct CMMessage;
struct SendReceiveClient;

class SendReceiveBackend : public Backend,
                           public tsd::common::system::Thread
{
   public:
      SendReceiveBackend(CommunicationManager &cm, tsd::common::logging::Logger &logger,
                         const std::string &address, bool global);
      virtual ~SendReceiveBackend();

      virtual void run();

   private:
      bool handlePulse(struct _pulse *pulse);
      void handleMessage(int rcvid, CMMessage *msg, struct _msg_info *info);
      void registerClient(struct _msg_info *info);
      void deregisterClients(int32_t scoid);
      void clientSendMessage(int rcvid, CMMessage *msg, SendReceiveClient *client);

      CommunicationManager &m_cm;
      name_attach_t *m_attach;
      tsd::common::logging::Logger &m_log;

      typedef std::map<uint64_t, SendReceiveClient*> ClientMap;
      ClientMap m_clients;
};

}}

#endif
