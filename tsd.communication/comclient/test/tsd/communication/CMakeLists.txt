#BUILD_TEST(Buffer STDMAIN NOGLOB
#    BufferTest.cpp
#    MallocHelper.c
#)

BUILD_TEST(TsdEventSerializer STDMAIN NOGLOB
    TsdEventSerializerTest.cpp
    MallocHelper.c
)

BUILD_TEST(Connection STDMAIN NOGLOB
    ConnectionTest.cpp
    net/ServerSocket.cpp
    net/Socket.cpp
)

BUILD_TEST(TcpConnection STDMAIN NOGLOB
    TcpConnectionTest.cpp
    net/ServerSocket.cpp
    net/ClientSocket.cpp
    net/Socket.cpp
)

