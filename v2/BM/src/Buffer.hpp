/*
 * Buffer.hpp
 *
 *  Created on: 06.09.2014
 *      Author: joern
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_
#include <stdint.h>

struct Buffer
{
  unsigned char *const bytes;
  const uint16_t size;

  Buffer(void *ptr, uint16_t size) : bytes((unsigned char*)ptr), size(size)
  {};

  unsigned char& operator[] (const uint16_t index) {
    return bytes[index];
  }
};

#endif /* BUFFER_HPP_ */
