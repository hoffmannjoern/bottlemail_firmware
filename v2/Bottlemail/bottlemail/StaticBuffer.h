//
//  StaticBuffer.hpp
//  bottlemail
//
//  Created by Jörn Hoffmann on 13.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

#ifndef _StaticBuffer_hpp
#define _StaticBuffer_hpp
#include "Buffer.h"

template <uint16_t SIZE>

/**
 * Struct to manage n amount of bytes defined by a template parameter
 */
struct StaticBuffer : Buffer
{
  unsigned char buffer[SIZE];

  StaticBuffer() : Buffer(buffer, SIZE)
  {}
};

#endif
