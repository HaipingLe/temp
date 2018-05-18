// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.hpp"

class ServerSocket: private Socket {
public:

	ServerSocket(uint16_t port);
	ServerSocket() {
	}
	;

	virtual ~ServerSocket();
	int getSocketId() const {
		return Socket::getSocketId();
	}

	const ServerSocket& operator <<(const std::string&) const;
	const ServerSocket& operator >>(std::string&) const;

	void accept(ServerSocket&);

};

#endif
