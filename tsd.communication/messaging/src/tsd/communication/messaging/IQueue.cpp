
#include <tsd/communication/messaging/IQueue.hpp>

namespace tsd { namespace communication { namespace messaging {

std::vector<uint32_t> IMessageFactory::getEventIds() const
{
   return std::vector<uint32_t>();
}

IQueue::~IQueue()
{
}

} } }
