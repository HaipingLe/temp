set(SOURCES
   public/tsd/communication/messaging/AddressInUseException.hpp
   public/tsd/communication/messaging/BaseException.hpp
   public/tsd/communication/messaging/Connection.hpp
   public/tsd/communication/messaging/ConnectionException.hpp
   public/tsd/communication/messaging/ILocalIfc.hpp
   public/tsd/communication/messaging/IQueue.hpp
   public/tsd/communication/messaging/IRemoteIfc.hpp
   public/tsd/communication/messaging/InvalidArgumentException.hpp
   public/tsd/communication/messaging/Queue.hpp
   public/tsd/communication/messaging/types.hpp

   src/tsd/communication/messaging/Connection.cpp
   src/tsd/communication/messaging/ConnectionImpl.cpp
   src/tsd/communication/messaging/ConnectionImpl.hpp
   src/tsd/communication/messaging/ILocalIfc.cpp
   src/tsd/communication/messaging/IPort.cpp
   src/tsd/communication/messaging/IPort.hpp
   src/tsd/communication/messaging/IQueue.cpp
   src/tsd/communication/messaging/IRemoteIfc.cpp
   src/tsd/communication/messaging/NameServer.cpp
   src/tsd/communication/messaging/NameServer.hpp
   src/tsd/communication/messaging/Packet.cpp
   src/tsd/communication/messaging/Packet.hpp
   src/tsd/communication/messaging/Queue.cpp
   src/tsd/communication/messaging/QueueInternal.hpp
   src/tsd/communication/messaging/Router.cpp
   src/tsd/communication/messaging/Router.hpp
   src/tsd/communication/messaging/utils.hpp
   )

if(TARGET_OS_POSIX_LINUX)
   set(SOURCES ${SOURCES}
      src/tsd/communication/messaging/Select.cpp
      src/tsd/communication/messaging/Select.hpp
      src/tsd/communication/messaging/SelectEpoll.cpp
      src/tsd/communication/messaging/SelectEpoll.hpp
      src/tsd/communication/messaging/SocketHelper.cpp
      src/tsd/communication/messaging/SocketHelper.hpp
      src/tsd/communication/messaging/TcpClientPort.cpp
      src/tsd/communication/messaging/TcpClientPort.hpp
      src/tsd/communication/messaging/TcpEndpoint.cpp
      src/tsd/communication/messaging/TcpEndpoint.hpp
      src/tsd/communication/messaging/TcpServerPort.cpp
      src/tsd/communication/messaging/TcpServerPort.hpp
      src/tsd/communication/messaging/UioShmPort.cpp
      src/tsd/communication/messaging/UioShmPort.hpp
      )
endif()

build_artifact(lib ${SOURCES})
module_include_directories(
   PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/public"
   PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")
module_dependencies(PUBLIC
   tsd.common
   tsd.communication.event
   )

add_subdirectory(app)
add_subdirectory(test)
