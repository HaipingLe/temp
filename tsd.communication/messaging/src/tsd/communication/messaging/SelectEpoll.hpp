/**
 * @file SelectEpoll.hpp
 *
 * Epoll based event loop interface.
 */

#ifndef TSD_COMMUNICATION_MESSAGING_SELECTEPOLL_HPP
#define TSD_COMMUNICATION_MESSAGING_SELECTEPOLL_HPP

#include <deque>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/types/typedef.hpp>

#include "Select.hpp"

namespace tsd { namespace communication { namespace messaging {

/**
 * Select implementation with epoll().
 *
 * The inner core uses the Linux epoll(7) interface. A eventfd is use to
 * unblock the dispatch loop from interrupt().
 *
 * We use the kernel to hold the reference to the SelectSourceImpl objects so
 * that we can dispatch witout going through a translation. OTOH we have to
 * make sure that these objects are around as long as the kernel holds a
 * reference.
 */
class SelectImpl
{
   typedef std::deque<SelectSourceImpl*> PendingQueue;

   tsd::common::logging::Logger m_log;
   int m_epollFd;
   int m_wakeFd;
   PendingQueue m_pending;

public:
   SelectImpl();
   ~SelectImpl();

   bool init();
   void dispatch();
   void interrupt();
   SelectSource* add(int fd, ISelectEventHandler *handler, bool selectRead,
      bool selectWrite);

   bool registerSource(int fd, SelectSourceImpl *source, uint32_t events);
   void unregisterSource(int fd, SelectSourceImpl *source);
};

/**
 * Imlementation class for SelectSource.
 *
 * Nothing more than a dumb structure to hold all relevant information about a
 * file descriptor in the event loop.
 */
class SelectSourceImpl
{
   SelectSource m_source;
   SelectImpl *m_selectImpl;
   ISelectEventHandler *m_handler;
   int m_fd;
   uint32_t m_events;
   uint32_t m_pendingEvents;
   bool m_active;
   bool m_dispatching;

   void unregister();

public:
   SelectSourceImpl(SelectImpl *selectImpl, int fd, ISelectEventHandler *handler, uint32_t events);
   ~SelectSourceImpl();

   bool init();
   void dispose();

   inline SelectSource* getSource()
   {
      return &m_source;
   }

   inline bool setPending(int events)
   {
      bool ret = m_pendingEvents == 0u;
      m_pendingEvents |= events;
      return ret;
   }

   void dispatch();
};

} } }

#endif
