#ifndef __BMMessage_hpp__
#define __BMMessage_hpp__

#include <stdint.h>

/*
 * The assumed package format is:
 * <cmd(1), len(1), payload(len), crc(2)>
 */

typedef struct
{
  uint8_t cmd;
  uint8_t payloadSize;
}BMMessageHeader;

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
  int writePayload(const uint8_t *payload, uint8_t size);
  int appendPayload(uint8_t byte);
  
  // Checksum
  uint16_t getCRC();
  void setCRC();
  void setCRC(uint16_t crc);
  
  // Checksum helper
  uint16_t computeCRC();
  bool isCRCValid();
  
  // Sizes
  uint16_t getSize();
  uint16_t getSizeWithoutCRC();
  
private:
  uint8_t  *_buffer;
  uint16_t  _bufferSize;
  uint8_t   _payloadSizeMax;
};

#endif