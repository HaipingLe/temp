#ifndef TSD_COMMUNICATION_MESSAGING_UTILS_HPP
#define TSD_COMMUNICATION_MESSAGING_UTILS_HPP

#include <tsd/communication/event/TsdEvent.hpp>

namespace tsd { namespace communication { namespace messaging {

const int INTERFACE_ADDR_SIZE = 24;
const tsd::communication::event::IfcAddr_t INTERFACE_ADDR_MASK = (UINT64_C(1) << INTERFACE_ADDR_SIZE) - 1u;
const int HOST_ADDR_SIZE = 64 - INTERFACE_ADDR_SIZE;
const tsd::communication::event::IfcAddr_t HOST_ADDR_MASK = (~UINT64_C(0)) << INTERFACE_ADDR_SIZE;
const tsd::communication::event::IfcAddr_t INVALID_ADDR_MASK = ~UINT64_C(0);

inline tsd::communication::event::IfcAddr_t getHostAddr(tsd::communication::event::IfcAddr_t addr)
{
   return addr & HOST_ADDR_MASK;
}

inline tsd::communication::event::IfcAddr_t getInterface(tsd::communication::event::IfcAddr_t addr)
{
   return addr & INTERFACE_ADDR_MASK;
}

inline bool isHostAddr(tsd::communication::event::IfcAddr_t addr, tsd::communication::event::IfcAddr_t host)
{
   return getHostAddr(addr) == getHostAddr(host);
}

inline bool isLocalAddr(tsd::communication::event::IfcAddr_t addr)
{
   return isHostAddr(addr, 0);
}

inline bool isNetworkAddr(tsd::communication::event::IfcAddr_t addr, tsd::communication::event::IfcAddr_t net, tsd::communication::event::IfcAddr_t mask)
{
   return (addr & mask) == (net & mask);
}

} } }

#endif
