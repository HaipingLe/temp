#ifndef TSD_COMMUNICATION_MESSAGING_IREMOTEIFCMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_IREMOTEIFCMOCK_HPP

#include <tsd/communication/messaging/IRemoteIfc.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace tsd {
namespace communication {
namespace messaging {

class IRemoteIfcMock : public IRemoteIfc
{
   public:
      IRemoteIfcMock(tsd::communication::event::IfcAddr_t localIfcAddr,
                     tsd::communication::event::IfcAddr_t remoteIfcAddr): IRemoteIfc(localIfcAddr, remoteIfcAddr) {}
      virtual ~IRemoteIfcMock(){}

      void sendMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg)
      {
          sendMessage(*msg);
      }
      MOCK_METHOD1(sendMessage,
                   void(tsd::communication::event::TsdEvent& msg));


      MonitorRef_t monitor(std::auto_ptr<tsd::communication::event::TsdEvent> event)
      {
          return monitor(*event);
      }
      MOCK_METHOD1(monitor,
                   MonitorRef_t(tsd::communication::event::TsdEvent& event));


      MOCK_METHOD1(demonitor,
                   void(MonitorRef_t monitor));
};

}
}
}

#endif
