//////////////////////////////////////////////////////////////////////
/// @file IQueueTest.hpp
/// @author piotr.brzyski@partner.preh.de
/// @brief Header file for Unit Tests to test IQueue
///
/// Copyright (c) Preh Car Connect GmbH
/// CONFIDENTIAL
//////////////////////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_MESSAGING_IQUEUETEST_HPP
#define TSD_COMMUNICATION_MESSAGING_IQUEUETEST_HPP

#include <cppunit/extensions/HelperMacros.h>

namespace tsd {
namespace communication {
namespace messaging {

// bug for two classes in one file: PRE06-5790

/**
 * Testclass for IMessageFactory
 *
 * @brief Testclass for IMessageFactory
 */
class IMessageFactoryTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::IQueue::GetEventIds
    * @tsd_testexpected std vector returned
    */
   void test_GetEventIds_JustRun_StdVectorReturned();

   CPPUNIT_TEST_SUITE(IMessageFactoryTest);
   CPPUNIT_TEST(test_GetEventIds_JustRun_StdVectorReturned);
   CPPUNIT_TEST_SUITE_END();
};

/**
 * Testclass for IQueue
 *
 * @brief Testclass for IQueue
 */
class IQueueTest : public CPPUNIT_NS::TestFixture
{
public:
   /**
    * @brief Test scenario: just run
    *
    * @tsd_testobject tsd::communication::messaging::IQueue::Destructor
    * @tsd_testexpected object destroyed
    */
   void test_Destructor_JustRun_ObjectDestroyed();

   CPPUNIT_TEST_SUITE(IQueueTest);
   CPPUNIT_TEST(test_Destructor_JustRun_ObjectDestroyed);
   CPPUNIT_TEST_SUITE_END();
};

} // namespace messaging
} // namespace communication
} // namespace tsd

#endif // TSD_COMMUNICATION_MESSAGING_IQUEUETEST_HPP
