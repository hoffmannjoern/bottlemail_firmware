//
//  CobsFrame.h
//  bottlemail
//
//  Created by Jörn Hoffmann on 14.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

#ifndef __bottlemail__CobsFrame__
#define __bottlemail__CobsFrame__

#include "Buffer.hpp"
#include <stdint.h>

class CobsFrame
{
  
public:

  /**
   * 
   */
  CobsFrame(Buffer &buffer, uint16_t length, bool encoded = false) : buffer(buffer), length(length), encoded(encoded)
  {};
  
  /**
   * Encodes the frame with cobs.
   * @return Returns true on success, false otherise.
   */
  bool encode();
  
  /**
   * Decodes the frame with cobs.
   * @return Returns true on success, false otherise.
   */
  bool decode();
  
  /**
   * Retuns the length of the frame.
   */
  unsigned long getLength() {
    return length;
  }
  
  /**
   * Returns true if the frame is encoded
   */
  bool isEncoded() {
    return encoded;
  }
  
private:
  Buffer &buffer;
  uint16_t length;
  bool encoded;
};


#endif /* defined(__bottlemail__CobsFrame__) */
