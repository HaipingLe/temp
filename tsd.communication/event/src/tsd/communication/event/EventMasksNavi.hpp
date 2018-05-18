//////////////////////////////////////////////////////////////////////
//!\file   tsd/communication/event/EventMasksNavi.hpp
//!\brief  Declaration of event mask for navigation ipc event for communication by communication client / server
//!\author o.flatau@technisat.de
//!
//!\polarion_workitem test workitem
//!
//!Copyright (c) TechniSat Digital GmbH
//!CONFIDENTIAL
//////////////////////////////////////////////////////////////////////
#ifndef _TSD_COMMUNICATION_EVENT_EVENTMASKSNAVI_HPP_
#define _TSD_COMMUNICATION_EVENT_EVENTMASKSNAVI_HPP_

#include <tsd/common/types/typedef.hpp>

namespace tsd { namespace communication { namespace event {

const uint32_t OFFSET_NAVEVENT_BASE        = 0x00000; //!< Navi base functions event mask
const uint32_t OFFSET_NAVEVENT_GUIDANCE    = 0x01000; //!< Guidance event mask
const uint32_t OFFSET_NAVEVENT_INPUT       = 0x02000; //!< Input event mask
const uint32_t OFFSET_NAVEVENT_MAPDISPLAY  = 0x03000; //!< Mapdisplay event mask
const uint32_t OFFSET_NAVEVENT_TRAFFIC     = 0x04000; //!< Traffic event mask
const uint32_t OFFSET_NAVEVENT_MEMORY      = 0x05000; //!< Memory event mask
const uint32_t OFFSET_NAVEVENT_MAINTENANCE = 0x06000; //!< Maintentance event mask
const uint32_t OFFSET_NAVEVENT_PSD         = 0x07000; //!< PSD event mask
const uint32_t OFFSET_NAVEVENT_PREDNAV     = 0x08000; //!< PredNav event mask

///////////////////////////////////////////////////////////
// Using as shown in following examples
///////////////////////////////////////////////////////////
//
// You can use it in form of enum:
//
//    enum TSDEVENT_NAV_GUIDANCE_Events
//    {
//       TSDEVENTID_NAV_GUIDANCE_NEWPOS_EVENT = tsd::communication::event::OFFSET_TSDEVENT_NAV
//                                            | tsd::communication::event::OFFSET_NAVEVENT_GUIDANCE + 1U
//       TSDEVENTID_NAV_GUIDANCE_START_EVENT,
//       TSDEVENTID_NAV_GUIDANCE_STOP_EVENT,
//       TSDEVENTID_NAV_GUIDANCE_UNKNOWN_EVENT
//    };
//
// Or define for each event an own variable:
//
//    const uint32_t idNewPos = tsd::communication::event::OFFSET_TSDEVENT_NAV | tsd::communication::event::OFFSET_NAVEVENT_GUIDANCE + 1U;
//    const uint32_t idNewPos = tsd::communication::event::OFFSET_TSDEVENT_NAV | tsd::communication::event::OFFSET_NAVEVENT_GUIDANCE + 2U;
//    const uint32_t idNewPos = tsd::communication::event::OFFSET_TSDEVENT_NAV | tsd::communication::event::OFFSET_NAVEVENT_GUIDANCE + 3U;

} /* namespace events */ } /* namespace communication */ } /* namespace tsd */

#endif // #ifndef(_TSD_COMMUNICATION_EVENT_EVENTMASKSNAVI_HPP_)
