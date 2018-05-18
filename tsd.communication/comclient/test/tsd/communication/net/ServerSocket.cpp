// Implementation of the ServerSocket class

#include <errno.h>

#include "ServerSocket.hpp"
#include "SocketException.hpp"
#include "EOFException.hpp"

ServerSocket::ServerSocket(uint16_t port) {
	if (!Socket::create()) {
		throw SocketException("Could not create server socket.", errno);
	}

	if (!Socket::bind(port)) {
		throw SocketException("Could not bind to port.", errno);
	}

	if (!Socket::listen()) {
		throw SocketException("Could not listen to socket.", errno);
	}

}

ServerSocket::~ServerSocket() {
}

const ServerSocket& ServerSocket::operator <<(const std::string& s) const {
	if (!Socket::send(s)) {
		throw SocketException("Could not write to socket.", errno);
	}

	return *this;

}

const ServerSocket& ServerSocket::operator >>(std::string& s) const {
	int result = Socket::recv(s);

	if (result == 0) {
		throw EOFException("End of stream.");
	} else if (result < 0) {
		throw SocketException("Could not read from socket.", errno);
	}

	return *this;
}

void ServerSocket::accept(ServerSocket& sock) {
	if (!Socket::accept(sock)) {
		throw SocketException("Could not accept socket.", errno);
	}
}
