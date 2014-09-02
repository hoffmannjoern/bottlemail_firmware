#include "BMMessage.hpp"
#include "crc.hpp"
#include <string.h>

const uint8_t HEADER_SIZE = sizeof(BMMessageHeader);
const uint8_t OVERHEAD_SIZE = HEADER_SIZE + sizeof(uint16_t);
#define HEADER(ptr) ((BMMessageHeader*)ptr)

BMMessage::BMMessage(uint8_t *buffer, uint16_t bufferSize)
{
  if (bufferSize >= OVERHEAD_SIZE)
  {
    _buffer = buffer;
    _bufferSize = bufferSize;
    
    // Set the max payload size in contructor due to not compute the bounds later
    uint16_t payloadSizeMax = bufferSize - OVERHEAD_SIZE;
    _payloadSizeMax = payloadSizeMax < UINT8_MAX ? payloadSizeMax : UINT8_MAX;
  }
  
  else {
    _buffer = NULL;
    _bufferSize = 0;
    _payloadSizeMax = 0;
  }
};

uint8_t BMMessage::getCmd()
{
  return HEADER(_buffer)->cmd;
};

void BMMessage::setCmd(uint8_t cmd)
{
  if (_buffer && _bufferSize >= HEADER_SIZE)
    HEADER(_buffer)->cmd = cmd;
}

uint8_t* BMMessage::getPayload()
{
  if (!getPayloadSize())
    return NULL;
  
  return _buffer + sizeof(BMMessageHeader);
 }

uint8_t BMMessage::getPayloadSize()
{
  return HEADER(_buffer)->payloadSize;
};

int BMMessage::writePayload(const uint8_t *newPayload, uint8_t payloadSize)
{
  // Check unsufficient space
  if (payloadSize > _payloadSizeMax)
    return -1;

  HEADER(_buffer)->payloadSize = payloadSize;
  memcpy(getPayload(), newPayload, payloadSize);
  return 0;
};

int BMMessage::appendPayload(uint8_t byte)
{
  // Check unsufficient space
  if (HEADER(_buffer)->payloadSize >= _payloadSizeMax)
    return -1;
  
  uint8_t *payload = getPayload();
  payload[HEADER(_buffer)->payloadSize++] = byte;
  return 0;
}

uint16_t BMMessage::getCRC()
{
  // Get the crc value that is behind the payload
  uint16_t crcIndex = getSizeWithoutCRC();
  return ((uint16_t)_buffer[crcIndex] << 8) | ((uint16_t) _buffer[crcIndex + 1]);
}

void BMMessage::setCRC(uint16_t crc)
{
  if (getSize() > _bufferSize)
    return;
  
  uint16_t crcIndex = getSizeWithoutCRC();
  _buffer[crcIndex]   = (uint8_t) (crc >> 8);
  _buffer[crcIndex+1] = (uint8_t) crc;
}

uint16_t BMMessage::computeCRC()
{
  return crc16_ccitt(_buffer, getSizeWithoutCRC());
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