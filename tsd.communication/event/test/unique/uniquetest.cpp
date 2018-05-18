/**
  *  \file uniquetest.cpp
  *  \brief Generated Test Case
  *  \author s.pietschmann@technisat.de
  *
  *  Copyright (c) TechniSat Digital GmbH
  *  CONFIDENTIAL
  */
#include "uniquetest.hpp"
#include <tsd/communication/event/EventMasksAdm.hpp>

/* This tests uniqueness of values within sets-of-constants.
   This can be easily done by letting the compiler generate code for a switch() statement.
   Duplicate cases are a compiler error. */

CPPUNIT_TEST_SUITE_REGISTRATION(UniqueTest);

/** Uniqueness test for "TSDEVENTID_ADM_XXX". */
void
UniqueTest::testPrefix1()
{
   switch (0) {
    case tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_RX: break;
    case tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_TX: break;
    case tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CC_EVENTS: break;
    case tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CM_EVENTS: break;
    case tsd::communication::event::TSDEVENTID_ADM_HELO: break;
    case tsd::communication::event::TSDEVENTID_ADM_INIT_CM_EVENTS: break;
    case tsd::communication::event::TSDEVENTID_ADM_PING: break;
    case tsd::communication::event::TSDEVENTID_ADM_PONG: break;
    case tsd::communication::event::TSDEVENTID_ADM_REGISTER_CC_EVENTS: break;
    case tsd::communication::event::TSDEVENTID_ADM_REGISTER_CM_EVENTS: break;
    case tsd::communication::event::TSDEVENTID_ADM_UNKNOWN: break;
   }
}


