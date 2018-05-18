//////////////////////////////////////////////////////////////////////
//! Copyright (c) 2011
//! TechniSat Digital GmbH
//!
//! \file    tsd/communication/event/EventMasksAdm.hpp
//! \author  Stefan Zill
//! \brief   Declaration of TSDEVENT_ADM_Events enum
//!
//////////////////////////////////////////////////////////////////////
#ifndef __EVENTMASKS_HPP_
#define __EVENTMASKS_HPP_

#include <tsd/common/types/typedef.hpp>

namespace tsd { namespace communication { namespace event {

//////////////////////////////////////////////////////////////////////////
// Event masks -> responsible allocator
//////////////////////////////////////////////////////////////////////////

const uint32_t TSDEVENT_MASK               = 0xFF000000U; //!< bitmask for top level event IDs
const uint32_t OFFSET_TSDEVENT_ADM         = 0x01000000U; //!< administrative event mask -> Rene Reusner
const uint32_t OFFSET_TSDEVENT_HMI         = 0x02000000U; //!< HMI event mask -> Stefan Zill
const uint32_t OFFSET_TSDEVENT_NAV         = 0x03000000U; //!< Navi event mask -> Oliver Flatau
const uint32_t OFFSET_TSDEVENT_RADIO       = 0x04000000U; //!< Radio event mask -> Daniel Jorczyk
const uint32_t OFFSET_TSDEVENT_PHONE       = 0x05000000U; //!< Phone event mask -> Frank Hennig
const uint32_t OFFSET_TSDEVENT_MEDIA       = 0x06000000U; //!< Media event mask -> Stefan Reuther
const uint32_t OFFSET_TSDEVENT_SYSTEM      = 0x07000000U; //!< System event mask -> Tobias John
const uint32_t OFFSET_TSDEVENT_CARCOM      = 0x08000000U; //!< CarCom event mask -> Matthias Rudolf
const uint32_t OFFSET_TSDEVENT_CONN        = 0x09000000U; //!< Connectivity event mask -> Marco Strauss
const uint32_t OFFSET_TSDEVENT_BLUETOOTH   = 0x0A000000U; //!< Blauzahn event mask -> Tobias Patzelt
const uint32_t OFFSET_TSDEVENT_AUDIO       = 0x0B000000U; //!< Audio event mask -> Olaf Schmidt
const uint32_t OFFSET_TSDEVENT_ORGANIZER   = 0x0C000000U; //!< Organizer event mask -> Joerg Fischer
const uint32_t OFFSET_TSDEVENT_MESSAGING   = 0x0D000000U; //!< Messaging event mask -> Stephan Wolf
const uint32_t OFFSET_TSDEVENT_SDS         = 0x0E000000U; //!< DEPRECATED -> move to ASR
const uint32_t OFFSET_TSDEVENT_ASR         = 0x0E000000U; //!< Speech Dialog System event mask -> Enrico Maasch
const uint32_t OFFSET_TSDEVENT_TTS         = 0x0F000000U; //!< Text To Speech event mask -> Christoph Haschlar
const uint32_t OFFSET_TSDEVENT_POS         = 0x10000000U; //!< Positioning -> Thomas Foerster
const uint32_t OFFSET_TSDEVENT_SPEECH      = 0x11000000U; //!< Speech common event mask -> Tim Susa
const uint32_t OFFSET_TSDEVENT_STATIONLOGO = 0x12000000U; //!< station logos event mask -> Michael Doecke
const uint32_t OFFSET_TSDEVENT_TRAVELLINK  = 0x13000000U; //!< Travel-Link event mask -> Michael Doecke
const uint32_t OFFSET_TSDEVENT_ROOT        = 0x14000000U; //!< root event mask -> Dennis Wassenberg
const uint32_t OFFSET_TSDEVENT_ASIAINPUT   = 0x15000000U; //!< AsiaInput event mask -> Frank Heuschkel


} /* namespace event */ } /* namespace communication */ } /* namespace tsd */

#endif
