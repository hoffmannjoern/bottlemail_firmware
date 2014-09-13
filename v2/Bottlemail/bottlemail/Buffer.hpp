/*
 * Buffer.hpp
 *
 *  Created on: 06.09.2014
 *      Author: joern
 */

#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__
#include <stdint.h>
#include <string.h>

struct Buffer
{
  unsigned char *const bytes;
  const uint16_t size;
  
  /**
   * Manages the given memory region.
   * @param ptr The pointer to the memory region to manage.
   * @param size The size of the memory region to manage.
   */
  Buffer(void *ptr, uint16_t size) : bytes((unsigned char*)ptr), size(size)
  {};
  
  /**
   * Make a deep copy of the given buffer.
   * \param other The buffer with the managed memory to copy.
   * @note The count of copied bytes is the minimum of the size.
   */
  void copy(Buffer &other)
  {
    memcpy(bytes, other.bytes, size < other.size ? size : other.size);
  }
  
  virtual ~Buffer()
  {}

  unsigned char& operator[] (const uint16_t index) {
    return bytes[index];
  }
};

#endif /* _BUFFER_HPP_ */
