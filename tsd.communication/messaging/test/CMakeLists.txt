set(COMMON_SRC
   InternalPort.cpp
   InternalPort.hpp
   MessageDefs.cpp
   MessageDefs.hpp
)

build_test(client-server NOGLOB STDMAIN ${COMMON_SRC}
   ClientServer.cpp
   INCDIRS ${MODULE_PATH}/src )

build_test(pub-sub NOGLOB STDMAIN ${COMMON_SRC}
   PubSub.cpp
   INCDIRS ${MODULE_PATH}/src )

build_test(nameserver NOGLOB STDMAIN ${COMMON_SRC}
   NameServer.cpp
   INCDIRS ${MODULE_PATH}/src )

build_test(monitor NOGLOB STDMAIN ${COMMON_SRC}
   Monitor.cpp
   INCDIRS ${MODULE_PATH}/src )

build_test(shm NOGLOB STDMAIN ${COMMON_SRC}
   ShmConnection.cpp TransportSuite.hpp TransportSuite.cpp
   INCDIRS ${MODULE_PATH}/src )

build_test(timers NOGLOB STDMAIN ${COMMON_SRC}
   Timers.cpp
   INCDIRS ${MODULE_PATH}/src )

# TCP, Unix and UIO are Linux only
if(TARGET_OS_POSIX_LINUX)
   build_test(tcp NOGLOB STDMAIN ${COMMON_SRC}
      TcpConnection.cpp TransportSuite.hpp TransportSuite.cpp
      INCDIRS ${MODULE_PATH}/src )
   build_test(unix NOGLOB STDMAIN ${COMMON_SRC}
      UnixConnection.cpp TransportSuite.hpp TransportSuite.cpp
      INCDIRS ${MODULE_PATH}/src )

   # the UIO test assumes /dev/uio[01] to be available
   if(TARGET_TYPE_EMBEDDED)
      build_test(uio NOGLOB STDMAIN ${COMMON_SRC}
         UioConnection.cpp TransportSuite.hpp TransportSuite.cpp
         INCDIRS ${MODULE_PATH}/src )
   endif()
endif()
