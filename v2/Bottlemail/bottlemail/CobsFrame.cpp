//
//  CobsFrame.cpp
//  bottlemail
//
//  Created by Jörn Hoffmann on 14.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

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
  
  // Encode
  bool success = cobsEncode(buffer.bytes, length, buffer.bytes, buffer.size);
  if (success) {
    encoded = true;
    length++;
  }
  
  return success;
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
  bool success = cobsDecode(buffer.bytes, length, buffer.bytes, buffer.size);
  if (success) {
    encoded = false;
    length--;
  }
  
  return success;
}
