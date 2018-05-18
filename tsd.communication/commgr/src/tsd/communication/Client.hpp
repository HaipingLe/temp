/**
 * \file Client.hpp
 * \brief Abstract client class definition
 * \author jan.kloetzke@technisat.de
 *
 * Copyright (c) TechniSat Digital GmbH
 * CONFIDENTIAL
 */
#ifndef __CLIENT_HPP_
#define __CLIENT_HPP_

#include <map>
#include <set>
#include <string>

namespace tsd { namespace communication {

class Client {
public:
   Client()
      : isManager(false)
      , receiveEnabled(true)
      , transmitEnabled(true)
      , name("unknown")
      , pid(0)
      , m_watchdogChallenge(0)
      , m_watchdogPending(0xffff)
   { }
   virtual ~Client() { }

   virtual void pushMessage(Buffer *msg) = 0;

   std::map<uint32_t, uint32_t> events;

   bool isManager;
   bool receiveEnabled;
   bool transmitEnabled;

   std::string name;
   int32_t pid;

   uint32_t m_watchdogChallenge;
   uint32_t m_watchdogPending;
};

}}

#endif
