#ifndef TSD_COMMUNICATION_MESSAGING_CONNECTIONIMPL_HPP
#define TSD_COMMUNICATION_MESSAGING_CONNECTIONIMPL_HPP

#include <deque>
#include <vector>

#include <tsd/common/ctassert.hpp>
#include <tsd/communication/messaging/Connection.hpp>

#include "IPort.hpp"

namespace tsd { namespace communication { namespace messaging {

   class Packet;

   class ConnectionImpl
      : private IPort
   {
      typedef std::deque<Packet*> PacketQueue;

      struct PacketHeader {
         tsd::common::ipc::NetworkInteger<uint32_t>   m_msgLen;
         tsd::common::ipc::NetworkInteger<uint32_t>   m_eventId;
         tsd::common::ipc::NetworkInteger<uint64_t>   m_senderAddr;
         tsd::common::ipc::NetworkInteger<uint64_t>   m_receiverAddr;
         uint8_t                                      m_type;
         uint8_t                                      m_padding[7];
      };
      compile_time_assert(sizeof(PacketHeader) == 32);

      IConnectionCallbacks &m_cb;
      tsd::common::system::Mutex m_lock;
      bool m_connected;

      PacketQueue m_outgoingQueue;
      Packet *m_outgoingPkt;
      size_t m_outgoingOffset;
      PacketHeader m_outgoingHdr;

      std::vector<uint8_t> m_incomingPkt;
      PacketHeader m_incomingHdr;
      size_t m_incomingHdrSize;

      void nextPacket();
      void processPacket(const uint8_t* p);
      void purgeQueue();

      bool startPort(); // IPort
      void stopPort(); // IPort
      bool sendPacket(std::auto_ptr<Packet> pkt); // IPort

   public:
      ConnectionImpl(IConnectionCallbacks &cb, Router &router);
      ~ConnectionImpl();

      void setConnected();
      void setDisconnected();

      void processData(const void* data, size_t size);
      bool getData(const void*& data, size_t& size);
      void consumeData(size_t amount);

      bool connectUpstream(const std::string &subDomain);
      bool listenDownstream();
      void disconnect();
   };

} } }

#endif
