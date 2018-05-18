/**
 * \file EventsTraffic.hpp
 *
 * \date 11.04.2012
 * \author ulbricht
 * \brief Defines events used by traffic to communicate with other modules via ipc.
 **/

#ifndef EVENTSTRAFFIC_HPP_
#define EVENTSTRAFFIC_HPP_

#include <tsd/communication/event/EventMasksNavi.hpp>
#include <tsd/communication/event/EventMasks.hpp>

namespace tsd { namespace communication { namespace event {

enum TSDEVENT_NAV_TRAFFIC
{
   TSDEVENT_NAV_TRAFFIC_SHELL_INPUT = (tsd::communication::event::OFFSET_TSDEVENT_NAV | tsd::communication::event::OFFSET_NAVEVENT_TRAFFIC) + 0U,
   TSDEVENT_NAV_TRAFFIC_RDS_INPUT = (tsd::communication::event::OFFSET_TSDEVENT_NAV | tsd::communication::event::OFFSET_NAVEVENT_TRAFFIC) + 1U,
};

} /* namespace events */ } /* namespace communication */ } /* namespace tsd */

#endif /* EVENTSTRAFFIC_HPP_ */
