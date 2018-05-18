#ifndef TRANSPORTSUITE_HPP
#define TRANSPORTSUITE_HPP

#include <string>

#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>

#include <tsd/communication/messaging/Connection.hpp>

namespace tsd { namespace communication { namespace messaging {

class Router;

class TransportSuite
   : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(TransportSuite);
   CPPUNIT_TEST(test_connect_stub);
   CPPUNIT_TEST(test_connect_auth);
   CPPUNIT_TEST(test_bulk_single);
   CPPUNIT_TEST(test_ping_huge);
   CPPUNIT_TEST_SUITE_END_ABSTRACT();

protected:
   virtual tsd::communication::messaging::IConnection* createConnection(Router &north,
      Router &south, const std::string &subDomain = "") = 0;

public:
   class ConnectFactory;

   virtual ~TransportSuite();

   /*
    * Regular tests. They are always executed and every transport should
    * survive them.
    */

   void test_connect_stub();
   void test_connect_auth();
   void test_bulk_single();
   void test_ping_huge();

   /*
    * Optional tests. Some of them may no make sense for all transports. Must
    * be registered manually via CPPUNIT_TEST() in the deriving class.
    */

   void test_bulk_multi();
   void test_bulk_connect_disconnect();
};

} } }

#endif
