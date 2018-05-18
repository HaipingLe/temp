#ifndef TSD_COMMUNICATION_MESSAGING_ILOCALIFCMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_ILOCALIFCMOCK_HPP

#include <tsd/communication/messaging/ILocalIfc.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace tsd {
namespace communication {
namespace messaging {

class ILocalIfcMock : public ILocalIfc
{
   public:
      ILocalIfcMock(tsd::communication::event::IfcAddr_t localIfcAddr) : ILocalIfc(localIfcAddr)
      {}
      virtual ~ILocalIfcMock(){}

      void sendMessage(event::IfcAddr_t remoteIfc, std::auto_ptr<event::TsdEvent> msg)
      {
          sendMessage(remoteIfc, *msg);
      }
      MOCK_METHOD2(sendMessage,
                  void(tsd::communication::event::IfcAddr_t remoteIfc, tsd::communication::event::TsdEvent& msg));


      void broadcastMessage(std::auto_ptr<event::TsdEvent> msg)
      {
          broadcastMessage(*msg);
      }
      MOCK_METHOD1(broadcastMessage,
                   void(tsd::communication::event::TsdEvent& msg));

      MonitorRef_t monitor(std::auto_ptr<event::TsdEvent> event, event::IfcAddr_t addr)
      {
          monitor(*event, addr);
      }
      MOCK_METHOD2(monitor,
                   MonitorRef_t(tsd::communication::event::TsdEvent& event, tsd::communication::event::IfcAddr_t addr));

      MOCK_METHOD1(demonitor,
                   void(MonitorRef_t monitor));
};

}
}
}

#endif
