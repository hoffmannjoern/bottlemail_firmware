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


/**
 * @class CobsFrame
 * Class to handle a buffer that contains a CobsFrame.
 */
class CobsFrame
{
  
public:
  /**
   * Initializes the class with the given buffer and frame length.
   * @param buffer The buffer that contains a cobs frame.
   * @param lentgth The length of the cobs frame.
   * @param encoded Value to define if the buffer was encoded by cobs or contains pure data.
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
  
  /**
   * The managed buffer.
   */
  Buffer &buffer;
  
private:
  uint16_t length;
  bool encoded;
};


#endif /* defined(__bottlemail__CobsFrame__) */
