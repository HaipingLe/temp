//////////////////////////////////////////////////////////////////////
/// @file QueueTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test Queue
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_QUEUETEST_HPP
#define TSD_COMMUNICATION_MESSAGING_QUEUETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

/**
 * Testclass for GlobalQueue
 *
 * @brief Testclass for GlobalQueue
 */
class GlobalQueueTest : public CPPUNIT_NS::TestFixture
{
public:

   /**
    * @brief tearDown function called after every test
    */
   void tearDown() override;

   /**
    * @brief Test scenario: invoke provided name
    *
    * @tsd_testobject tsd::communication::messaging::GlobalQueue::CreateQueue
    * @tsd_testexpected expecting valid pointer returned
    */
   void test_CreateQueue_InvokeProvidedName_ExpectingValidPointerReturned();

   CPPUNIT_TEST_SUITE(GlobalQueueTest);
   CPPUNIT_TEST(test_CreateQueue_InvokeProvidedName_ExpectingValidPointerReturned);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_QUEUETEST_HPP
