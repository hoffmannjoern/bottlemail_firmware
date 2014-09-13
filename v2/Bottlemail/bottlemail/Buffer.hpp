/*
 * Buffer.hpp
 *
 *  Created on: 06.09.2014
 *      Author: joern
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_
#include <stdint.h>
#include <stdlib.h>

struct Buffer
{
  unsigned char *const bytes;
  const uint16_t size;
  
  Buffer(void *ptr, uint16_t size) : bytes((unsigned char*)ptr), size(size)
  {};
  
  virtual ~Buffer()
  {}

  unsigned char& operator[] (const uint16_t index) {
    return bytes[index];
  }
};


struct DynamicBuffer : Buffer
{
  DynamicBuffer(uint16_t aSize) : Buffer(malloc(aSize), aSize)
  { }
  
  virtual ~DynamicBuffer() {
    free(bytes);
  }
};

template <uint16_t SIZE>
struct StaticBuffer : Buffer
{
  unsigned char buffer[SIZE];
  StaticBuffer() : Buffer(buffer, SIZE)
  {}
};


#endif /* BUFFER_HPP_ */
