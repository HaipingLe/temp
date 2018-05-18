/**
 * @file Select.hpp
 *
 * Event loop interface definition
 */
#ifndef TSD_COMMUNICATION_MESSAGING_SELECT_HPP
#define TSD_COMMUNICATION_MESSAGING_SELECT_HPP

namespace tsd { namespace communication { namespace messaging {

class ISelectEventHandler
{
public:
   virtual ~ISelectEventHandler();

   virtual bool selectReadable() = 0;
   virtual bool selectWritable() = 0;
   virtual bool selectError() = 0;
};

class SelectImpl;
class SelectSource;
class SelectSourceImpl;

/**
 * Event loop handler and dispatcher.
 *
 * The whole class is essentially single threaded. All methods must be called
 * from a single thread only. The only exception is the interrupt() method that
 * can unblock the thread that is calling dispatch().
 *
 * Before destructing the object all event sources must have been disposed.
 */
class Select
{
   SelectImpl *m_impl;

public:
   Select();
   ~Select();

   /**
    * Initialize event loop.
    *
    * Must be called as first method. Upon succesfull return the other methods
    * can be used.
    *
    * @return Initialization status
    */
   bool init();

   /**
    * Wait for events and dispatch them.
    *
    * This method will wait for file descritor events and call the registered
    * ISelectEventHandler callbacks accordingly. It will only return if
    * interrupt() is called from another thread.
    */
   void dispatch();


   /**
    * Unblock dispatch method.
    *
    * Makes the dispatch method return to the caller. Might be called from
    * another thread.
    */
   void interrupt();

   /**
    * Add file descriptor to event loop.
    *
    * The file descriptor is watched in edge-triggered mode. That is the event
    * is only triggered once and automatically re-enabled if the read/write
    * functions returned EAGAIN/EWOULDBLOCK.
    *
    * @attention The file descritor must not be closed before calling dispose()
    * on the returned SelectSource. Otherwise the OS might allocate the file
    * descritor number again for a new object while the Select object holds the
    * old reference.
    */
   SelectSource* add(int fd, ISelectEventHandler *handler, bool selectRead = true,
      bool selectWrite = true);
};

/**
 * Select event source handle.
 *
 * An object of this class represents an active file descriptor in the event
 * loop.  Desipite disposing it the user cannot do anything with it. The object
 * is owned by the user. Do not delete it directly but call dispose() to safely
 * free the associated resources.
 */
class SelectSource
{
   friend class SelectSourceImpl;

   SelectSourceImpl *m_impl;

   /**
    * Private constructor.
    *
    * This object is intended to be contructed from the SelectSourceImpl.
    */
   SelectSource(SelectSourceImpl *impl);

   /**
    * Private destructor.
    *
    * A user has to call dispose() to free the SelectSource. The
    * SelectSourceImpl implementation will free it as soon as all internal
    * references have been dropped.
    */
   ~SelectSource();

public:

   /**
    * Dispose event source.
    *
    * After calling this method the application will no longer get callbacks
    * from the Select::dispatch() method. The application may close the
    * associated file descriptor immediately after calling this method.
    *
    * This method may be called from an ISelectEventHandler callback.
    *
    * @attention The associated file descritor must not be closed before
    * calling this method. Otherwise the OS might allocate the file descritor
    * number again for a new object while the Select object holds the old
    * reference.
    */
   void dispose();
};

} } }

#endif
