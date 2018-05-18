#include <iostream>
#include <tsd/common/types/typedef.hpp>
#include <tsd/communication/Connection.hpp>
#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Thread.hpp>

class Listener : public tsd::communication::client::IReceiveCallback
{
public:
   Listener(const std::string &name)
      : m_name(name)
   {
   }

   virtual void messageReceived(const void * /*buf*/, uint32_t len)
   {
      std::cout << "messageReceived " << m_name << " len:" << len << &std::endl;
   }

   virtual void disconnected()
   {
      std::cout << "disconnected " << m_name << &std::endl;
   }

   std::string m_name;
};

int main(int argc, char* argv[])
{
   Listener l1("one");
   Listener l2("two");

   tsd::common::logging::Logger log("mcp.client");

   tsd::communication::client::Connection::openConnection(&l1, log, NULL);
   tsd::communication::client::Connection::openConnection(&l1, log, NULL);

   for (;;)
      tsd::common::system::Thread::sleep(10);

   return 0;
}
