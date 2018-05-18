#ifndef TSD_COMMUNICATION_MESSAGING_QUEUEINTERNAL_HPP
#define TSD_COMMUNICATION_MESSAGING_QUEUEINTERNAL_HPP

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/CondVar.hpp>
#include <tsd/common/system/Mutex.hpp>
#include <tsd/communication/event/TsdEvent.hpp>

#include <tsd/communication/messaging/Queue.hpp>

namespace tsd { namespace communication { namespace messaging {

class Router;
class Packet;
class IIfcNotifiy;

class Queue
   : public IQueue
{
   struct EventSlot {
      tsd::communication::event::TsdEvent *m_event;
      uint32_t m_ref;
   };
   typedef std::deque<EventSlot> EventQueue;
   typedef std::map<tsd::communication::event::IfcAddr_t, const IMessageFactory*> InterfaceFactories;
   typedef std::map<tsd::communication::event::IfcAddr_t, IIfcNotifiy*> InterfaceNotifications;

   struct Timer {
      tsd::communication::event::TsdEvent *m_event;
      uint32_t m_expires;
      uint32_t m_interval;
      TimerRef_t m_ref;

      inline bool operator<(const Timer& rhs) const
      {
         return m_expires < rhs.m_expires;
      }
   };
   typedef std::multiset<Timer> Timers;

   std::string m_name;
   tsd::common::logging::Logger m_log;
   tsd::common::system::Mutex m_lock;
   tsd::common::system::CondVar m_queueCondition;
   EventQueue m_queue;
   InterfaceFactories m_ifcFactories;
   InterfaceNotifications m_ifcNotifications;
   Router &m_router;
   uint32_t m_numInterfaces;
   uint32_t m_refSeqNum;
   Timers m_timers;

   std::auto_ptr<tsd::communication::event::TsdEvent> pullMessage(IMessageSelector *selector);
   uint32_t checkTimerExpired(uint32_t now);

public:
   Queue(const std::string &name, Router &router);
   ~Queue();

   ILocalIfc *registerInterface(const IMessageFactory &factory,
                                const std::string &interfaceName = "");

   IRemoteIfc *connectInterface(const std::string &interfaceName,
                               const IMessageFactory &factory,
                               uint32_t timeout = INFINITE_TIMEOUT);

   IRemoteIfc *connectInterface(const ILocalIfc *localIfc,
                               const IMessageFactory &factory);

   std::auto_ptr<tsd::communication::event::TsdEvent> readMessage(
      uint32_t timeout = INFINITE_TIMEOUT,
      IMessageSelector *selector = NULL);
   void sendSelfMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg);
   TimerRef_t startTimer(std::auto_ptr<tsd::communication::event::TsdEvent> event, uint32_t ms, bool cyclic);
   bool stopTimer(TimerRef_t timer);

   // iternal methods

   inline std::string getName() const { return m_name; }

   void interfaceAdded(tsd::communication::event::IfcAddr_t ifc, IIfcNotifiy *notify, const IMessageFactory &factory);
   void interfaceRemoved(tsd::communication::event::IfcAddr_t ifc);

   void pushMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg,
                    uint32_t ref = 0, bool multicast = false);
   bool pushPacket(std::auto_ptr<Packet> evt, bool multicast);
   void purgeMessages(uint32_t ref);
   inline Router& getRouter() { return m_router; }

   IRemoteIfc *connectInterface(tsd::communication::event::IfcAddr_t remoteAddr,
                                const IMessageFactory &factory);

   uint32_t makeRef();
   void dead(tsd::communication::event::IfcAddr_t sender,
             tsd::communication::event::IfcAddr_t receiver);
};

} } }

#endif
