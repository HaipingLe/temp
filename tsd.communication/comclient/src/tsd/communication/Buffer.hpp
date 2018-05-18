///////////////////////////////////////////////////////
//!\file Buffer.hpp
//!\brief Buffer (allocator) definition
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_BUFFER_HPP
#define TSD_COMMUNICATION_BUFFER_HPP

#include <cstdlib>
#include <cstring>
#include <tsd/common/system/AtomicInteger.hpp>
#include <tsd/common/types/typedef.hpp>
#include <tsd/common/ipc/rpcbuffer.h>

namespace tsd { namespace communication {

struct TSD_COMMUNICATION_COMCLIENT_DLLEXPORT Buffer {
      inline uint32_t eventId()
      {
         tsd::common::ipc::RpcBuffer buf;
         buf.init((char*)payload(), length());
         return buf.getInt();
      }

      inline void *payload()
      {
         return m_payload;
      }

      inline uint32_t length()
      {
         return m_length;
      }

      inline void *payloadWithHeader()
      {
         return &m_length;
      }

      inline uint32_t lengthWithHeader()
      {
         return m_length + 4;
      }

      inline uint32_t fill(const void *buf, uint32_t len)
      {
         uint32_t remaining = m_length - m_index;
         remaining = remaining > len ? len : remaining;
         std::memcpy((uint8_t*)m_payload + m_index, buf, remaining);
         m_index += remaining;
         return remaining;
      }

      inline bool isFilled()
      {
         return m_index >= m_length;
      }

      inline void ref()
      {
         m_refcnt.increment();
      }

      void deref()
      {
         if (m_refcnt.decrement() == 1) {
            std::free(this);
         }
      }

      tsd::common::system::AtomicInteger m_refcnt;
      uint32_t m_index;
      uint32_t m_timestamp;

      uint32_t m_length;
      uint32_t m_payload[1];
};

TSD_COMMUNICATION_COMCLIENT_DLLEXPORT
Buffer *allocBuffer(uint32_t len);

}}

#endif
