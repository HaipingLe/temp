#FILE(GLOB_RECURSE SOURCES *.[ch]pp)
SET(SOURCES_COMMON
   tsd/communication/Buffer.cpp
   tsd/communication/Buffer.hpp
   tsd/communication/Communication.cpp
   tsd/communication/Communication.hpp
   tsd/communication/CommunicationClient.cpp
   tsd/communication/CommunicationClient.hpp
   tsd/communication/Connection.cpp
   tsd/communication/Connection.hpp
   tsd/communication/DispatchAdapter.cpp
   tsd/communication/DispatchAdapter.hpp
   tsd/communication/IComReceive.hpp
   tsd/communication/ICommunicationClient.cpp
   tsd/communication/ICommunicationClient.hpp
   tsd/communication/IEventSerializer.cpp
   tsd/communication/IEventSerializer.hpp
   tsd/communication/TsdEventSerializer.cpp
   tsd/communication/TsdEventSerializer.hpp
)

IF(TARGET_OS_POSIX_QNX)
   SET(SOURCES
      ${SOURCES_COMMON}
			tsd/communication/TcpConnection.cpp
      tsd/communication/TcpConnection.hpp
      tsd/communication/QnxConnection.cpp
      tsd/communication/QnxConnection.hpp
   )
ELSEIF (TARGET_OS_POSIX OR TARGET_OS_CYGWIN)
    SET(SOURCES
       ${SOURCES_COMMON}
       tsd/communication/TcpConnection.cpp
       tsd/communication/TcpConnection.hpp
    )
ELSEIF (TARGET_OS_WIN32)
    SET(SOURCES
       ${SOURCES_COMMON}
       tsd/communication/TcpConnectionWindows.cpp
       tsd/communication/TcpConnectionWindows.hpp
    )
ELSE()
   SET(SOURCES ${SOURCES_COMMON})
ENDIF()

VS_GROUP_SOURCES("${SOURCES}" ${CMAKE_CURRENT_SOURCE_DIR})

BUILD_ARTIFACT("lib" ${SOURCES})
