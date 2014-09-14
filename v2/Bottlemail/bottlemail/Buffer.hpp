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

/**
 * Struct to manage and access a memory region more safer.
 */
struct Buffer
{
  /**
   * The pointer to the memory region.
   */
  unsigned char *const bytes;
  
  /**
   * The size of the memory region.
   */
  const uint16_t size;
  
  /**
   * Manages the given memory region.
   * @param ptr The pointer to the memory region to manage.
   * @param size The size of the memory region to manage.
   */
  Buffer(void *ptr, uint16_t size) : bytes((unsigned char*)ptr), size(size)
  {}
  
  /**
   * Make a deep copy of the given buffer.
   * \param other The buffer with the managed memory to copy.
   * @note The count of copied bytes is the minimum size, either of the other or this buffer.
   */
  void copy(Buffer &other)
  {
    memcpy(bytes, other.bytes, size < other.size ? size : other.size);
  }
  
  /**
   * Checks if the buffer is valid
   * @note A buffer if only valid if it has a non zero size and a non zero byte ptr.
   * @return Returns true if valid, false otherwise.
   */
  bool isValid()
  {
    if (size != 0 && bytes != NULL)
      return true;
      
    return false;
  }
  
  virtual ~Buffer()
  {}

  /**
   * The array access operator to directly manipulate the managed memory region.
   */
  unsigned char& operator[] (const uint16_t index)
  {
    return bytes[index];
  }
};

#endif /* _BUFFER_HPP_ */
