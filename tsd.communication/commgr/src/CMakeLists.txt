SET( SOURCES_COMMON
      tsd/communication/Client.hpp
      tsd/communication/CommunicationManager.cpp
      tsd/communication/CommunicationManager.hpp
      tsd/communication/DownstreamManager.cpp
      tsd/communication/DownstreamManager.hpp
      tsd/communication/IComWatchdog.cpp
      tsd/communication/IComWatchdog.hpp
      )

IF(TARGET_OS_POSIX_QNX)
   SET(SOURCES
      ${SOURCES_COMMON}
       tsd/communication/TcpBackend.cpp
       tsd/communication/TcpBackend.hpp
      tsd/communication/SendReceiveBackend.cpp
      tsd/communication/SendReceiveBackend.hpp
   )
ELSEIF (TARGET_OS_POSIX)
    SET(SOURCES
       ${SOURCES_COMMON}
       tsd/communication/TcpBackend.cpp
       tsd/communication/TcpBackend.hpp
    )
ELSEIF (TARGET_OS_WIN32)
    SET(SOURCES
       ${SOURCES_COMMON}
       tsd/communication/TcpBackendWindows.cpp
       tsd/communication/TcpBackendWindows.hpp
    )
ELSE()
   SET(SOURCES ${SOURCES_COMMON})
ENDIF()

BUILD_ARTIFACT(lib ${SOURCES})
