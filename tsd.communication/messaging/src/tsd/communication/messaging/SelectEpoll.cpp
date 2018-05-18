/**
 * @file SelectEpoll.cpp
 *
 * Epoll based event loop implementation.
 */

#include <cstring>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "SelectEpoll.hpp"

static const int MAX_EVENTS = 8;

namespace tsd { namespace communication { namespace messaging {

SelectImpl::SelectImpl()
   : m_log("tsd.communication.messaging.SelectImpl")
   , m_epollFd(-1)
   , m_wakeFd(-1)
{
}

SelectImpl::~SelectImpl()
{
   // No need to deregister active file descriptors. The kernel will discard
   // all resources when closing m_epollFd. Just the SelectSourceImpl objects
   // will be lost... :-o

   if (m_wakeFd != -1) {
      close(m_wakeFd);
   }
   if (m_epollFd != -1) {
      close(m_epollFd);
   }
}

/**
 * Initialize select loop.
 *
 * Allocate the necessary resources.
 */
bool SelectImpl::init()
{
   m_epollFd = epoll_create1(EPOLL_CLOEXEC);
   if (m_epollFd == -1) {
      m_log << tsd::common::logging::LogLevel::Error
            << "epoll_create1 failed w/ " << errno
            << &std::endl;
      goto err;
   }

   m_wakeFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
   if (m_wakeFd == -1) {
      m_log << tsd::common::logging::LogLevel::Error
            << "eventfd failed w/ " << errno
            << &std::endl;
      goto err_epfd;
   }

   struct epoll_event ev;
   std::memset(&ev, 0, sizeof(ev));
   ev.events = EPOLLIN;
   ev.data.ptr = NULL;
   if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_wakeFd, &ev) == -1) {
      m_log << tsd::common::logging::LogLevel::Error
            << "epoll_ctl(m_wakeFd) failed w/ " << errno
            << &std::endl;
      goto err_wakefd;
   }

   return true;

err_wakefd:
   close(m_wakeFd);
   m_wakeFd = -1;
err_epfd:
   close(m_epollFd);
   m_epollFd = -1;
err:
   return false;
}

/**
 * THE dispatch loop.
 *
 * We wait for the kernel to give us up to MAX_EVENTS events. We scan through
 * all returned events and mark the associated SelectSourceImpl objects as
 * pending. If the object was not pending yet we add it to our m_pending list
 * for later dispatching.  We don't immediately dispatch the events to handle
 * the case where the kernel returns multiple distinct events for the same file
 * descriptor and that the user is allowed to call SelectSourceImpl::dispose()
 * safely from a ISelectEventHandler callback.
 *
 * Once we have filled the m_pending list we dispatch every source one-by-one.
 * We do not iterate because some pending sources could vanish because the user
 * is allowed to call dispose() from the callbacks.
 */
void SelectImpl::dispatch()
{
   bool repeat = true;

   while (repeat) {
      // wait for next events
      struct epoll_event events[MAX_EVENTS];
      int nfds = epoll_wait(m_epollFd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
         if (errno != EINTR) {
            m_log << tsd::common::logging::LogLevel::Error
                  << "epoll_wait failed w/ " << errno
                  << &std::endl;
            break;
         }
         continue;
      }

      // roll through all events and mark them as pending
      for (int i = 0; i < nfds; i++) {
         SelectSourceImpl *source = reinterpret_cast<SelectSourceImpl*>(events[i].data.ptr);
         if (source == NULL) {
            eventfd_t unused;
            eventfd_read(m_wakeFd, &unused);
            repeat = false;
         } else {
            bool added = source->setPending(events[i].events);
            if (added) {
               m_pending.push_back(source);
            }
         }
      }

      // dispatch all events that are on the "pending" list
      while (!m_pending.empty()) {
         SelectSourceImpl *source = m_pending.front();
         m_pending.pop_front();

         source->dispatch();
      }
   }
}

/**
 * Unblock dispatch().
 */
void SelectImpl::interrupt()
{
   eventfd_write(m_wakeFd, 1);
}

/**
 * Add file descriptor to event loop.
 *
 * @param fd            The file descriptor to watch.
 * @param handler       Callback interface pointer.
 * @param selectRead    Watch for readability
 * @param selectWrite   Watch for writeability
 *
 * @return SelectSource handle or NULL if the handle could not be added
 */
SelectSource* SelectImpl::add(int fd, ISelectEventHandler *handler, bool selectRead,
      bool selectWrite)
{
   uint32_t events = EPOLLET;
   if (selectRead)  { events |= EPOLLIN;  }
   if (selectWrite) { events |= EPOLLOUT; }

   SelectSourceImpl *source = new SelectSourceImpl(this, fd, handler, events);
   if (source->init()) {
      return source->getSource();
   } else {
      return NULL;
   }
}

/**
 * Callback from SelectSourceImpl to add a file descriptor to loop.
 *
 * Does the actual call to the kernel to start monitoring the file descriptor.
 *
 * @param fd      The file descriptor to watch.
 * @param source  The SelectSourceImpl that represents the file descriptor
 * @param events  EPOLL* mask
 *
 * @return True if call succeeded, otherwise false.
 */
bool SelectImpl::registerSource(int fd, SelectSourceImpl *source, uint32_t events)
{
   int ret = 0;

   struct epoll_event ev;
   std::memset(&ev, 0, sizeof(ev));
   ev.events = events;
   ev.data.ptr = source;

   ret = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev);
   if (ret < 0) {
      m_log << tsd::common::logging::LogLevel::Error
            << "epoll_ctl(add) failed w/ " << errno
            << &std::endl;
   }

   return ret == 0;
}

/**
 * Remove file descriptor from kernel.
 *
 * If the @p source is not NULL it is assumed that the source is in the pending
 * list and must be pruned from it.
 *
 * @param fd    The file descriptor that is currently watched.
 * @param source  The SelectSourceImpl that represents the file descriptor.
 */
void SelectImpl::unregisterSource(int fd, SelectSourceImpl *source)
{
   // unregister in kernel
   (void)epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL);

   // filter m_pending if the source was pending
   if (source != NULL) {
      for (PendingQueue::iterator it(m_pending.begin()); it != m_pending.end(); ++it) {
         if (*it == source) {
            m_pending.erase(it);
            break;
         }
      }
   }
}

/*****************************************************************************/

SelectSourceImpl::SelectSourceImpl(SelectImpl *selectImpl, int fd, ISelectEventHandler *handler, uint32_t events)
   : m_source(this)
   , m_selectImpl(selectImpl)
   , m_handler(handler)
   , m_fd(fd)
   , m_events(events)
   , m_pendingEvents(0)
   , m_active(true)
   , m_dispatching(false)
{
}

SelectSourceImpl::~SelectSourceImpl()
{ }

bool SelectSourceImpl::init()
{
   return m_selectImpl->registerSource(m_fd, this, m_events);
}

/**
 * Dispatch the pending events of a source.
 *
 * We have to handle the situation that one of the callbacks calls back the
 * SelectSourceImpl::dispose() method. In this case the dispose() method sees
 * that m_dispatching is true and does not yet free the object. That's why we
 * have to check if m_active is false on the exit to finally delete us.
 *
 * If any of the callbacks returns false we stop watching the file descriptor.
 * The application still has to call dispose(), though.
 */
void SelectSourceImpl::dispatch()
{
   bool unreg = false;

   // watch out for the callback cycle...
   m_dispatching = true;

   if ((m_pendingEvents & EPOLLIN) != 0u) {
      if (!m_handler->selectReadable()) {
         unreg = true;
      }
   }
   if ((m_pendingEvents & EPOLLOUT) != 0u) {
      if (!m_handler->selectWritable()) {
         unreg = true;
      }
   }
   if ((m_pendingEvents & (EPOLLERR | EPOLLHUP)) != 0u) {
      if (!m_handler->selectError()) {
         unreg = true;
      }
   }

   // we're back
   m_dispatching = false;
   m_pendingEvents = 0u;

   // A handler returned an error. Unregister the source so that no events can
   // be fired anymore.
   if (unreg) {
      unregister();
   }

   // Was the source disposed while we were dispatching? If yes it is already
   // unregistered and we have to finally free it.
   if (!m_active) {
      delete this;
   }
}

/**
 * Free event source.
 *
 * Extra care must be taken if this is called through a callback from
 * dispatch().
 */
void SelectSourceImpl::dispose()
{
   if (m_active) {
      m_active = false;
      unregister();

      // Special handling if dispose() is called while dispatching. We
      // unregistered the source already so it is ok for the application to
      // close the file descriptor upon return from this method. We have to
      // keep the object alive because SelectSourceImpl::dispatch() still has a
      // pointer to it, though.
      if (!m_dispatching) {
         delete this;
      }
   }
}

/**
 * Make sure the file descriptor is not watched anymore.
 */
void SelectSourceImpl::unregister()
{
   if (m_events != 0u) {
      m_selectImpl->unregisterSource(m_fd, m_pendingEvents != 0 ? this : NULL);
      m_events = 0u;
      m_pendingEvents = 0u;
   }
}

} } }
