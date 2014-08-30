#ifndef __BMMessage_hpp__
#define __BMMessage_hpp__

#include <stdint.h>
#include "crc.hpp"

typedef struct _BMMessage
{
  uint16_t crc;
  uint8_t  cmd;
  uint8_t  payloadSize;
  
  // ---------------------------------------------------------------------------------------------------------------------------------------- //
  uint8_t *getPayload()
  {
    return &payloadSize + 1;
  }
  
  void setPayload(const uint8_t *aPayload, uint8_t aSize)
  {
    payloadSize = aSize;
    memcpy(getPayload(), aPayload, aSize);
  }
  
  uint16_t getSize()
  {
    return sizeof(_BMMessage) + payloadSize;
  }
  
  uint16_t getSizeWithoutCrc()
  {
    return sizeof(_BMMessage) - sizeof(crc) + payloadSize;
  }

  uint16_t computeCRC()
  {
    return crc16_ccitt(&cmd, getSizeWithoutCrc());
  }
  
  void setCRC(uint16_t aCrc)
  {
    crc = htons(aCrc);
  }
  
  uint16_t getCRC()
  {
    return ntohs(crc);
  }
  
  bool isCRCValid()
  {   
    return computeCRC() == getCRC();
  }
  
} __attribute__((packed)) BMMessage;

#endif