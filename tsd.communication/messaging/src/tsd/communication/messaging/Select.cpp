/**
 * @file Select.cpp
 *
 * Event loop implementation, generic part.
 */

#include "Select.hpp"
#include "SelectEpoll.hpp"

namespace tsd { namespace communication { namespace messaging {

ISelectEventHandler::~ISelectEventHandler()
{
}

Select::Select()
   : m_impl(new SelectImpl)
{
}

Select::~Select()
{
   delete m_impl;
}

bool Select::init()
{
   return m_impl->init();
}

void Select::dispatch()
{
   m_impl->dispatch();
}

void Select::interrupt()
{
   m_impl->interrupt();
}

SelectSource* Select::add(int fd, ISelectEventHandler *handler, bool selectRead,
      bool selectWrite)
{
   return m_impl->add(fd, handler, selectRead, selectWrite);
}

/*****************************************************************************/

SelectSource::SelectSource(SelectSourceImpl *impl)
   : m_impl(impl)
{
}

SelectSource::~SelectSource()
{
}

void SelectSource::dispose()
{
   m_impl->dispose();
}

} } }
