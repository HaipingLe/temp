/**
 * \file DownstreamManager.cpp
 * \brief Client implementation for downstream CM
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */

#include <tsd/communication/Buffer.hpp>
#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/communication/DownstreamManager.hpp>

using tsd::communication::DownstreamManager;

DownstreamManager::DownstreamManager(tsd::communication::CommunicationManager &cm,
                                     tsd::common::logging::Logger &logger,
                                     const std::string &address)
   : m_cm(cm)
   , m_logger(logger)
{
   isManager = true;
   name = "downstreamCM";
   m_connection = client::Connection::openConnection(this, logger, address.c_str());
}

DownstreamManager::~DownstreamManager()
{
   delete m_connection;
}

void DownstreamManager::pushMessage(tsd::communication::Buffer *msg)
{
   m_connection->send(msg);
}

void DownstreamManager::messageReceived(const void *buf, uint32_t len)
{
   Buffer *msg = allocBuffer(len);
   msg->fill(buf, len);

   m_cm.lock();
   m_cm.dispatchMessage(this, msg);
   m_cm.unlock();
}

void DownstreamManager::disconnected()
{
   m_logger << tsd::common::logging::LogLevel::Error
            << "Downstream CM disconnected!" << std::endl;
}
