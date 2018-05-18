#include <cppunit/CppUnit.hpp>

#include "MessageDefs.hpp"

// QuitInd ///////////////////////////////////////////////////////////////////

QuitInd::QuitInd()
   : tsd::communication::event::TsdEvent(QUIT_IND)
{ }

void QuitInd::serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const
{ }

void QuitInd::deserialize(tsd::common::ipc::RpcBuffer& /*buf*/)
{ }

tsd::communication::event::TsdEvent*
QuitInd::clone(void) const
{
   return new QuitInd;
}

// TimerInd //////////////////////////////////////////////////////////////////

TimerInd::TimerInd()
   : tsd::communication::event::TsdEvent(TIMER_IND)
   , m_id(0)
{ }

TimerInd::TimerInd(uint32_t id)
   : tsd::communication::event::TsdEvent(TIMER_IND)
   , m_id(id)
{ }

void TimerInd::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   buf << m_id;
}

void TimerInd::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   buf >> m_id;
}

tsd::communication::event::TsdEvent*
TimerInd::clone(void) const
{
   return new TimerInd(m_id);
}

// DummyInd //////////////////////////////////////////////////////////////////

DummyInd::DummyInd()
   : tsd::communication::event::TsdEvent(DUMMY_IND)
{ }

void DummyInd::serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const
{ }

void DummyInd::deserialize(tsd::common::ipc::RpcBuffer& /*buf*/)
{ }

tsd::communication::event::TsdEvent* DummyInd::clone(void) const
{
   return new DummyInd;
}

// FooInd ////////////////////////////////////////////////////////////////////

FooInd::FooInd()
   : tsd::communication::event::TsdEvent(FOO_IND)
{ }

void FooInd::serialize(tsd::common::ipc::RpcBuffer& /*buf*/) const
{ }

void FooInd::deserialize(tsd::common::ipc::RpcBuffer& /*buf*/)
{ }

tsd::communication::event::TsdEvent* FooInd::clone(void) const
{
   return new FooInd;
}

// FooInd ////////////////////////////////////////////////////////////////////

StringInd::StringInd()
   : tsd::communication::event::TsdEvent(STRING_IND)
{ }

StringInd::StringInd(const std::string &str)
   : tsd::communication::event::TsdEvent(STRING_IND)
   , m_string(str)
{ }

void StringInd::serialize(tsd::common::ipc::RpcBuffer& buf) const
{
   buf << m_string;
}

void StringInd::deserialize(tsd::common::ipc::RpcBuffer& buf)
{
   buf >> m_string;
}

tsd::communication::event::TsdEvent* StringInd::clone(void) const
{
   return new StringInd(m_string);
}

// SampleFactory /////////////////////////////////////////////////////////////

std::auto_ptr<tsd::communication::event::TsdEvent>
SampleFactory::createEvent(uint32_t msgId) const
{
   std::auto_ptr<tsd::communication::event::TsdEvent> ret;

   switch (msgId) {
      case DUMMY_IND:
         ret.reset(new DummyInd);
         break;
      case FOO_IND:
         ret.reset(new FooInd);
         break;
      case STRING_IND:
         ret.reset(new StringInd);
         break;
      default:
         CPPUNIT_ASSERT(false);
   }

   return ret;
}

tsd::communication::messaging::IMessageFactory&
SampleFactory::getInstance()
{
   static SampleFactory factory;
   return factory;
}

