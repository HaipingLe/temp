#ifndef MESSAGE_DEFS_HPP
#define MESSAGE_DEFS_HPP

#include <tsd/communication/messaging/IQueue.hpp>

static const uint32_t QUIT_IND = 1;
static const uint32_t TIMER_IND = 2;

class QuitInd
   : public tsd::communication::event::TsdEvent
{
public:
   QuitInd();
   void serialize(tsd::common::ipc::RpcBuffer& buf) const;
   void deserialize(tsd::common::ipc::RpcBuffer& buf);
   tsd::communication::event::TsdEvent* clone(void) const;
};

class TimerInd
   : public tsd::communication::event::TsdEvent
{
public:
   TimerInd();
   TimerInd(uint32_t id);
   void serialize(tsd::common::ipc::RpcBuffer& buf) const;
   void deserialize(tsd::common::ipc::RpcBuffer& buf);
   tsd::communication::event::TsdEvent* clone(void) const;

   uint32_t m_id;
};

/*
 * Broadcast interface
 */

static const uint32_t DUMMY_IND = 1;
static const uint32_t FOO_IND = 2;
static const uint32_t STRING_IND = 3;

class DummyInd
   : public tsd::communication::event::TsdEvent
{
public:
   DummyInd();
   void serialize(tsd::common::ipc::RpcBuffer& buf) const;
   void deserialize(tsd::common::ipc::RpcBuffer& buf);
   tsd::communication::event::TsdEvent* clone(void) const;
};

class FooInd
   : public tsd::communication::event::TsdEvent
{
public:
   FooInd();
   void serialize(tsd::common::ipc::RpcBuffer& buf) const;
   void deserialize(tsd::common::ipc::RpcBuffer& buf);
   tsd::communication::event::TsdEvent* clone(void) const;
};

class StringInd
   : public tsd::communication::event::TsdEvent
{
public:
   StringInd();
   StringInd(const std::string &str);
   void serialize(tsd::common::ipc::RpcBuffer& buf) const;
   void deserialize(tsd::common::ipc::RpcBuffer& buf);
   tsd::communication::event::TsdEvent* clone(void) const;

   std::string m_string;
};


class SampleFactory
   : public tsd::communication::messaging::IMessageFactory
{
public:
   std::auto_ptr<tsd::communication::event::TsdEvent> createEvent(uint32_t msgId) const;
   static tsd::communication::messaging::IMessageFactory& getInstance();
};


#endif
