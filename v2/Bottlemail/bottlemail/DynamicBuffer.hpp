//
//  DynamicBuffer.hpp
//  bottlemail
//
//  Created by Jörn Hoffmann on 13.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

#ifndef _DynamicBuffer_hpp
#define _DynamicBuffer_hpp
#include "Buffer.hpp"
#include <stdlib.h>

/**
 * Struct to manage an amount of bytes that is allocated on the heap.
 */
struct DynamicBuffer : Buffer
{
  /**
   * Creates an manages a memory region on the heap more safely.
   * @param aSize The size of the allocated memory region.
   */
  DynamicBuffer(uint16_t aSize) : Buffer(malloc(aSize), aSize)
  { }
  
  virtual ~DynamicBuffer() {
    free(bytes);
  }
};

#endif
