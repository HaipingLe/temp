#ifndef TSD_COMMUNICATION_MESSAGING_TCPENDPOINT_HPP
#define TSD_COMMUNICATION_MESSAGING_TCPENDPOINT_HPP

#include <deque>
#include <memory>

#include <tsd/common/logging/Logger.hpp>
#include <tsd/common/system/Mutex.hpp>

#include "Select.hpp"

namespace tsd { namespace communication { namespace messaging {

class Packet;

class TcpEndpoint
   : private ISelectEventHandler
{
   void received(const char *buffer);
   ssize_t sendPartialPacket(Packet *pkt, size_t offset);
   void setDisconnected();

   bool selectReadable();  // ISelectEventHandler
   bool selectWritable();  // ISelectEventHandler
   bool selectError();     // ISelectEventHandler

   tsd::common::logging::Logger &m_log;
   int m_socket;
   SelectSource* m_selectSource;
   tsd::common::system::Mutex m_lock;
   std::deque<Packet*> m_sendQueue;
   size_t m_sendOffset;

   char *m_buffer;
   uint32_t m_bufferSize;
   size_t m_inPtr;
   size_t m_outPtr;

   bool m_alive;

protected:
   bool epSendPacket(std::auto_ptr<Packet> pkt);
   virtual void epReceivedPacket(std::auto_ptr<Packet> pkt) = 0;
   virtual void epDisconnected() = 0;

public:
   TcpEndpoint(tsd::common::logging::Logger &log);
   virtual ~TcpEndpoint();

   bool init(int fd, Select &selector);
   void cleanup();
};

} } }

#endif
