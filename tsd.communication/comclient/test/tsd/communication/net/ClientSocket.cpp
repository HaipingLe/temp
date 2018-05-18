// Implementation of the ClientSocket class

#include <errno.h>

#include "ClientSocket.hpp"
#include "SocketException.hpp"
#include "EOFException.hpp"

ClientSocket::ClientSocket(std::string host, uint16_t port) {
	if (!Socket::create()) {
		throw SocketException("Could not create client socket.", errno);
	}

	if (!Socket::connect(host, port)) {
		throw SocketException("Could not bind to port.", errno);
	}

}

const ClientSocket& ClientSocket::operator <<(const std::string& s) const {
	if (!Socket::send(s)) {
		throw SocketException("Could not write to socket.", errno);
	}

	return *this;

}

const ClientSocket& ClientSocket::operator >>(std::string& s) const {
	int result = Socket::recv(s);

	if (result == 0) {
		throw EOFException("End of stream.");
	} else if (result < 0) {
		throw SocketException("Could not read from socket.", errno);
	}

	return *this;
}
