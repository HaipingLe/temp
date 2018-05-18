
#include <cstring>

#include <tsd/communication/messaging/ConnectionException.hpp>

#include "SocketHelper.hpp"

bool
tsd::communication::messaging::parseUnixPath(const std::string &path,
                                             struct sockaddr_un &sockaddr,
                                             socklen_t &sockaddrLen)
{
   bool abstract = false;

   if (path.length() > sizeof(sockaddr.sun_path)-1) {
      throw ConnectionException("Unix name too long: " + path);
   }

   std::memset(&sockaddr, 0, sizeof(sockaddr));
   sockaddr.sun_family = AF_UNIX;
   std::strncpy(sockaddr.sun_path, path.c_str(), sizeof(sockaddr.sun_path)-1);
   if (sockaddr.sun_path[0] == '@') {
      if (path.length() == 1) {
         // auto bind in linux abstract namespace
         sockaddrLen = sizeof(sa_family_t);
      } else {
         // linux abstract namespace
         sockaddr.sun_path[0] = '\0';
         sockaddrLen = static_cast<socklen_t>(sizeof(sa_family_t) + path.length());
      }
      abstract = true;
   } else {
      // include terminating NUL byte
      sockaddrLen = static_cast<socklen_t>(sizeof(sa_family_t) + path.length() + 1u);
   }

   return abstract;
}
