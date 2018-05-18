/**
  *  \file rangetest.cpp
  *  \brief Generated Test Case
  *  \author s.pietschmann@technisat.de
  *
  *  Copyright (c) TechniSat Digital GmbH
  *  CONFIDENTIAL
  */
#include "rangetest.hpp"
#include <tsd/communication/event/EventMasksAdm.hpp>

/* This tests ranges of values within sets-of-constants. */

CPPUNIT_TEST_SUITE_REGISTRATION(RangeTest);

/** Range test for "TSDEVENTID_ADM_XXX". */
void
RangeTest::testPrefix1()
{
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_RX >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_RX <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_TX >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEBUG_DISABLE_TX <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CC_EVENTS >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CC_EVENTS <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CM_EVENTS >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_DEREGISTER_CM_EVENTS <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_HELO >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_HELO <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_INIT_CM_EVENTS >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_INIT_CM_EVENTS <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_PING >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_PING <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_PONG >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_PONG <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_REGISTER_CC_EVENTS >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_REGISTER_CC_EVENTS <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_REGISTER_CM_EVENTS >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_REGISTER_CM_EVENTS <= 0x01FFFFFF);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_UNKNOWN >= 0x01000000);
   CPPUNIT_ASSERT(tsd::communication::event::TSDEVENTID_ADM_UNKNOWN <= 0x01FFFFFF);
}


