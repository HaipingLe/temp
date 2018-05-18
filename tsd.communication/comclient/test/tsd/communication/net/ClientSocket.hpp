// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "Socket.hpp"

class ClientSocket: private Socket {
public:

	ClientSocket(std::string host, uint16_t port);
	virtual ~ClientSocket() {};

	int getSocketId() const {
		return Socket::getSocketId();
	}

	const ClientSocket& operator <<(const std::string&) const;
	const ClientSocket& operator >>(std::string&) const;

};

#endif
