//
//  FillBuffer.hpp
//  bottlemail
//
//  Created by Jörn Hoffmann on 13.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

#ifndef _FillBuffer_hpp
#define _FillBuffer_hpp
#include "Buffer.hpp"

class FillBuffer : public Buffer
{
  
public:
  /**
   * Initializes a FillBuffer to manage the given memory region.
   * @param aPtr A pointer to the memory region to manage.
   * @param aSize Size of the memory region to manage.
   */
  FillBuffer(void *aPtr, uint16_t aSize) : Buffer(aPtr, aSize), pos((unsigned char*)aPtr)
  {}
  
  /**
   * Initializes a FillBuffer to operate on the given buffer.
   * @param aBuffer The buffer to operate on.
   * @note The buffer is NOT copied.
   */
  FillBuffer(Buffer &aBuffer) : FillBuffer(aBuffer.bytes, aBuffer.size)
  {}
  
  /**
   * Appends a byte to the current fill position
   */
  void appendByte(unsigned char byte)
  {
    // Set byte to current position
    uint16_t length = getLength();

    // Check if we are out of bounds
    if (length >= size) {
      pos = bytes;
      length = 0;
    }

    if (length < size)
      *pos++ = byte;
  }
  
  /**
   * Returns the count of filled bytes, thus the length of the buffer.
   * @note The maximum size of the buffer can be determined by the size variable @see size.
   */
  uint16_t getLength() {
    return pos - bytes;
  }
  
private:
  /// Current position to append byte.
  unsigned char *pos;
};

#endif
