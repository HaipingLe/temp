/**
 * \file DownstreamManager.hpp
 * \brief Client definition for downstream CM
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */
#ifndef __DOWNSTREAMMANAGER_HPP_
#define __DOWNSTREAMMANAGER_HPP_

#include <tsd/communication/Connection.hpp>
#include <tsd/communication/Client.hpp>

namespace tsd { namespace communication {

class CommunicationManager;

class DownstreamManager : public Client
                        , public client::IReceiveCallback
{
public:
   DownstreamManager(CommunicationManager &cm, tsd::common::logging::Logger &logger,
                     const std::string &address);
   virtual ~DownstreamManager();

   virtual void pushMessage(Buffer *msg); // Client
   virtual void messageReceived(const void *buf, uint32_t len); // client::IReceiveCallback
   virtual void disconnected(); // client::IReceiveCallback

private:
   DownstreamManager(const DownstreamManager&); // forbid copy ctor
   DownstreamManager& operator=(const DownstreamManager&); // forbid assignment operator

   CommunicationManager &m_cm;
   tsd::common::logging::Logger &m_logger;
   client::Connection *m_connection;
};

}}

#endif
