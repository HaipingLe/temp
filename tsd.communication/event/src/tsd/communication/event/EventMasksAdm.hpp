//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/EventMasksAdm.hpp
//! \author  Stefan Zill
//! \brief   Declaration of TSDEVENT_ADM_Events enum
//!
//////////////////////////////////////////////////////////////////////

#ifndef __EVENTMASKSADM_HPP_
#define __EVENTMASKSADM_HPP_

#include <tsd/communication/event/EventMasks.hpp>

namespace tsd { namespace communication { namespace event {

//////////////////////////////////////////////////////////////////////////
const uint32_t TSDEVENTID_ADM_REGISTER_CC_EVENTS   = OFFSET_TSDEVENT_ADM + 1U;
const uint32_t TSDEVENTID_ADM_DEREGISTER_CC_EVENTS = OFFSET_TSDEVENT_ADM + 2U;
const uint32_t TSDEVENTID_ADM_REGISTER_CM_EVENTS   = OFFSET_TSDEVENT_ADM + 3U;
const uint32_t TSDEVENTID_ADM_DEREGISTER_CM_EVENTS = OFFSET_TSDEVENT_ADM + 4U;
const uint32_t TSDEVENTID_ADM_HELO                 = OFFSET_TSDEVENT_ADM + 5U;
const uint32_t TSDEVENTID_ADM_INIT_CM_EVENTS       = OFFSET_TSDEVENT_ADM + 6U;
const uint32_t TSDEVENTID_ADM_DEBUG_DISABLE_RX     = OFFSET_TSDEVENT_ADM + 7U;
const uint32_t TSDEVENTID_ADM_DEBUG_DISABLE_TX     = OFFSET_TSDEVENT_ADM + 8U;
const uint32_t TSDEVENTID_ADM_PING                 = OFFSET_TSDEVENT_ADM + 9U;
const uint32_t TSDEVENTID_ADM_PONG                 = OFFSET_TSDEVENT_ADM + 10U;
const uint32_t TSDEVENTID_ADM_UNKNOWN              = OFFSET_TSDEVENT_ADM + 11U;
const uint32_t TSDEVENTID_ADM_WATCHDOG_EXPIRED     = OFFSET_TSDEVENT_ADM + 12U;

} /* namespace events */ } /* namespace communication */ } /* namespace tsd */

#endif
