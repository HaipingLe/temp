///////////////////////////////////////////////////////
//!\file Buffer.cpp
//!\brief Buffer allocator implementation
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <tsd/communication/Buffer.hpp>
#include <assert.h>

tsd::communication::Buffer *
tsd::communication::allocBuffer(uint32_t len)
{
   /* Because the Buffer struct contains the first element of an array (uint32_t m_payload[1])
    * and we want to allocate aligned blocks, we need to calculate the size by:
    *    1. subtracting sizeof(m_payload[1]) = 4 Byte
    *    2. round-up to the nearest multiple of 4
    *       Example:
    *          a) len = 3 Byte
    *
    *             > sizeof(Buffer) - 4 + (( 3 + 3 ) & 0xfffffffc
    *             > sizeof(Buffer) - 4 + ( 6 & 0xfffffffc )
    *             > sizeof(Buffer) - 4 + 4
    *             > sizeof(Buffer)
    *
    */
   Buffer *ret = (tsd::communication::Buffer *)std::malloc(sizeof(Buffer) - 4 + ((len + 3) & ~0x03));
   assert(ret != 0);

   ret->m_length = len;
   ret->m_refcnt = 1;
   ret->m_index = 0;
   return ret;
}

