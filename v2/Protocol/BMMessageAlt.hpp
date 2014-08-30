#ifndef __BMMessage_hpp__
#define __BMMessage_hpp__

#include <stdint.h>

class BMMessage
{

public:
  BMMessage(uint8_t *buffer, uint16_t bufferSize);
  
  // Command
  uint8_t getCmd();
  void setCmd(uint8_t cmd);
  
  // Payload
  uint8_t *getPayload();
  uint8_t getPayloadSize();
  void setPayload(const uint8_t *payload, uint8_t size);
  
  // Checksum
  uint16_t getCRC();
  void setCRC(uint16_t crc);
  uint16_t computeCRC();
  bool isCRCValid();

  // Sizes
  uint16_t getSize();
  uint16_t getSizeWithoutCRC();

  
private:
  uint8_t  *buffer;
  uint16_t  bufferSize;
};

#endif