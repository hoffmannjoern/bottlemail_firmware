#include "BMMessage.hpp"
#include "crc.hpp"
#include <stdint.h>
#include <string.h>

const uint8_t HEADER_SIZE = sizeof(BMMessageHeader);
const uint8_t OVERHEAD_SIZE = HEADER_SIZE + sizeof(uint16_t);
#define HEADER(ptr) ((BMMessageHeader*)ptr)

BMMessage::BMMessage(Buffer &buffer) : _buffer(buffer)
{
  // Set the max payload size in contructor due to not compute the bounds later
  if (buffer.size >= OVERHEAD_SIZE) {
    uint16_t payloadSizeMax = buffer.size - OVERHEAD_SIZE;
    _payloadSizeMax = payloadSizeMax < UINT8_MAX ? payloadSizeMax : UINT8_MAX;
  }
  
  else
    _payloadSizeMax = 0;
};

uint8_t BMMessage::getCmd()
{
  return HEADER(_buffer.bytes)->cmd;
};

void BMMessage::setCmd(uint8_t cmd)
{
  if (_buffer.size >= HEADER_SIZE)
    HEADER(_buffer.bytes)->cmd = cmd;
}

void* BMMessage::getPayload()
{
 return _buffer.bytes + HEADER_SIZE;
}

uint8_t BMMessage::getPayloadSize()
{
  return HEADER(_buffer.bytes)->payloadSize;
};

Buffer BMMessage::getPayloadBuffer()
{
  uint16_t payloadSize = getPayloadSize();
  if (payloadSize)
    return Buffer(getPayload(), payloadSize);

  else
    return Buffer(0,0);
}

int BMMessage::writePayload(const void *newPayload, uint8_t payloadSize)
{
  // Check insufficient space
  if (payloadSize > _payloadSizeMax)
    return -1;

  HEADER(_buffer.bytes)->payloadSize = payloadSize;
  memcpy(getPayload(), newPayload, payloadSize);
  return 0;
};

int BMMessage::appendPayload(unsigned char byte)
{
  // Check insufficient space
  if (HEADER(_buffer.bytes)->payloadSize >= _payloadSizeMax)
    return -1;
  
  char *payload = (char*) getPayload();
  payload[HEADER(_buffer.bytes)->payloadSize++] = byte;
  return 0;
}

uint16_t BMMessage::getCRC()
{
  // Get the crc value that is behind the payload
  uint16_t crcIndex = getSizeWithoutCRC();
  return ((uint16_t)_buffer[crcIndex] << 8) | ((uint16_t) _buffer[crcIndex + 1]);
}

void BMMessage::setCRC()
{
  setCRC(computeCRC());
}

void BMMessage::setCRC(uint16_t crc)
{
  if (getSize() > _buffer.size)
    return;
  
  uint16_t crcIndex = getSizeWithoutCRC();
  _buffer[crcIndex]   = (uint8_t) (crc >> 8);
  _buffer[crcIndex+1] = (uint8_t) crc;
}

uint16_t BMMessage::computeCRC()
{
  return crc16_ccitt(_buffer.bytes, getSizeWithoutCRC());
}

bool BMMessage::isCRCValid()
{
  return getCRC() == computeCRC();
}

uint16_t BMMessage::getSize()
{
  return OVERHEAD_SIZE + getPayloadSize();
}

uint16_t BMMessage::getSizeWithoutCRC()
{
  return HEADER_SIZE + getPayloadSize();
}
