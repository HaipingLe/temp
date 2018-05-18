#ifndef TSD_COMMUNICATION_MESSAGING_SOCKETHELPER_HPP
#define TSD_COMMUNICATION_MESSAGING_SOCKETHELPER_HPP

#include <string>
#include <sys/socket.h>
#include <sys/un.h>

namespace tsd { namespace communication { namespace messaging {

/**
 * Parse path into unix domain socket address structure.
 *
 * @param[in]     path        The path to parse
 * @param[in,out] sockaddr    The address structure that is filled
 * @param[out]    sockaddrLen Bytes filled in the socketaddr structure
 *
 * @return  If it is an abstract socket address or not (regular path).
 */
bool parseUnixPath(const std::string &path,
                   struct sockaddr_un &sockaddr,
                   socklen_t &sockaddrLen);

} } }

#endif

