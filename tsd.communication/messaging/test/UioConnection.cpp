
#include <UnitTest.hpp>
#include <cppunit/CppUnit.hpp>

#include <tsd/communication/messaging/UioShmPort.hpp>

#include "TransportSuite.hpp"


namespace tsd { namespace communication { namespace messaging {

   class UioConnection
      : public IConnection
   {
      UioShmPort *m_northPort;
      UioShmPort *m_southPort;

   public:
      UioConnection()
         : m_northPort(0)
         , m_southPort(0)
      { }

      ~UioConnection()
      {
         UioConnection::disconnect();
         if (m_northPort != 0) {
            delete m_northPort;
         }
         if (m_southPort != 0) {
            delete m_southPort;
         }
      }

      bool connect(Router &upstreamRouter, Router &downstreamRouter,
                   const std::string &subDomain)
      {
         m_northPort = new UioShmPort(upstreamRouter);
         m_northPort->listenDownstream("/dev/uio0");

         m_southPort = new UioShmPort(downstreamRouter);
         m_southPort->connectUpstream("/dev/uio1", subDomain);
      }

      // IConnection
      void disconnect()
      {
         if (m_northPort != 0) {
            m_northPort->disconnect();
         }
         if (m_southPort != 0) {
            m_southPort->disconnect();
         }
      }
   };

} } }

using namespace tsd::communication::messaging;

/*****************************************************************************/

/**
 * Transport test case for Linux UIO shared memory.
 *
 * This test assumes two UIO instances on the target (/dev/uio0 and /dev/uio1)
 * that are connected to each other. The test suite will only be registered
 * when building for the target because there is usually no UIO available on
 * the host.
 */
class UioConnectionTestSuite
   : public TransportSuite
{
   CPPUNIT_TEST_SUB_SUITE(UioConnectionTestSuite, TransportSuite);

   // no additional tests so far

   CPPUNIT_TEST_SUITE_END();

protected:
   // TransportSuite
   IConnection* createConnection(Router &north, Router &south, const std::string &subDomain)
   {
      std::auto_ptr<UioConnection> conn(new UioConnection);
      if (conn->connect(north, south, subDomain)) {
         return conn.release();
      } else {
         return NULL;
      }
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(UioConnectionTestSuite);
