// Definition of the Socket class

#ifndef Socket_class
#define Socket_class

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 65535;

class Socket {
public:
	Socket();
	virtual ~Socket();

	// Server initialization
	bool create();
	bool bind(const uint16_t port);
	bool listen() const;
	bool accept(Socket&) const;
	void close() const;
	int getSocketId() const;

	// Client initialization
	bool connect(const std::string host, const uint16_t port);

	// Data Transimission
	bool send(const std::string) const;
	int recv(std::string&) const;

	void setNonBlocking(const bool);

	bool isValid() const {
		return m_sock != -1;
	}

private:

	int m_sock;
	sockaddr_in m_addr;

};

#endif
