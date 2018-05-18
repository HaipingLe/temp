#ifndef TSD_COMMUNICATION_MESSAGING_IQUEUEMOCK_HPP
#define TSD_COMMUNICATION_MESSAGING_IQUEUEMOCK_HPP

#include <tsd/communication/messaging/IQueue.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace tsd {
namespace communication {
namespace messaging {


class IMessageFactoryMock : public IMessageFactory
{
   public:
      std::auto_ptr<tsd::communication::event::TsdEvent> createEvent(uint32_t msgId) const
      {
         std::auto_ptr< tsd::communication::event::TsdEvent>(createEventRelay(msgId));
      }
      MOCK_CONST_METHOD1(createEventRelay,  tsd::communication::event::TsdEvent * (uint32_t msgId));

      MOCK_CONST_METHOD0(getEventIds, std::vector<uint32_t>());
};

class IMessageSelectorMock : public  IMessageSelector
{
   MOCK_CONST_METHOD1(filterEvent, bool(tsd::communication::event::TsdEvent * event));
};

class IQueueMock : public IQueue
{
   public:
      IQueueMock(){}
      virtual ~IQueueMock(){};

      ILocalIfc *registerInterface(const IMessageFactory &factory)
      {
         return registerInterface(factory, "" );
      }
      MOCK_METHOD2(registerInterface,
                   ILocalIfc * (const IMessageFactory &factory,
                                const std::string & interfaceName));



      IRemoteIfc *connectInterface(const std::string &interfaceName,
                                   const IMessageFactory &factory)
      {
         return connectInterface(interfaceName,factory, INFINITE_TIMEOUT );
      }
      MOCK_METHOD3(connectInterface,
                   IRemoteIfc * (const std::string & interfaceName,
                                 const IMessageFactory &factory,
                                 uint32_t timeout));


      MOCK_METHOD2(connectInterface,
                   IRemoteIfc * (const ILocalIfc * localIfc, const IMessageFactory &factory));


      std::auto_ptr<tsd::communication::event::TsdEvent>
      readMessage(uint32_t timeout = INFINITE_TIMEOUT,
                  IMessageSelector *selector = NULL)
      {
         std::auto_ptr<tsd::communication::event::TsdEvent>(readMessageRelay(timeout, selector));
      }
      MOCK_METHOD2(readMessageRelay,
                   tsd::communication::event::TsdEvent * (uint32_t timeout,
                                                          IMessageSelector *selector));


      void sendSelfMessage(std::auto_ptr<tsd::communication::event::TsdEvent> msg)
      {
         sendSelfMessage(*msg);
      }
      MOCK_METHOD1(sendSelfMessage,
                   void(tsd::communication::event::TsdEvent & msg));


      TimerRef_t startTimer(std::auto_ptr<tsd::communication::event::TsdEvent> event, uint32_t ms, bool cyclic)
      {
         return startTimer(*event, ms, cyclic);
      }
      MOCK_METHOD3(startTimer,
                   TimerRef_t(tsd::communication::event::TsdEvent & event, uint32_t ms, bool cyclic));

      MOCK_METHOD1(stopTimer,
                   bool(TimerRef_t timer));
};

}
}
}

#endif
