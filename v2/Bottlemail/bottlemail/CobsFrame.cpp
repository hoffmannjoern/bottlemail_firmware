//
//  CobsFrame.cpp
//  bottlemail
//
//  Created by Jörn Hoffmann on 14.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

#include "DynamicBuffer.h"

#include "CobsFrame.h"
extern "C" {
  #include "cobs.h"
}

bool CobsFrame::encode()
{
  // Sanity checking
  if (encoded || length == 0)
    return true;
  else if (!buffer.isValid())
    return false;
  else if (length+1 > buffer.size)
    return false;

  
  // DynamicBuffer dybuffer(buffer.size);
  
  // Encode
  int result = cobs_encode(buffer.bytes, buffer.size, buffer.bytes, length);
  if (result) {
    encoded = true;
    length++;
    return true;
  }
  
  return false;
}

bool CobsFrame::decode()
{
  // Sanity checking
  if (!encoded || length == 0)
    return true;
  else if (!buffer.isValid())
    return false;
  else if (length > buffer.size)
    return false;

  // Decode
  int result = cobs_decode(buffer.bytes, buffer.size, buffer.bytes, length);
  if (result) {
    encoded = false;
    length--;
    return true;
  }
  
  return false;
}
