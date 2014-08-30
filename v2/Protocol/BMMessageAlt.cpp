#include "BMMessage.hpp"
#include "crc.hpp"
#include <string.h>

#define CMD_INDEX 0
#define PAYLOAD_SIZE_INDEX 1
#define PAYLOAD_INDEX 2

/**
 * The assumed package format is:
 * <cmd(1), len(1), payload(len), crc(2)>
 */

BMMessage::BMMessage(uint8_t *aBuffer, uint16_t aBufferSize) : buffer(aBuffer), bufferSize(aBufferSize)
{ };

uint8_t BMMessage::getCmd()
{
  return buffer[CMD_INDEX];
};

void BMMessage::setCmd(uint8_t cmd)
{
  buffer[CMD_INDEX] = cmd;
}

uint8_t* BMMessage::getPayload()
{
  return buffer + PAYLOAD_INDEX;
}

uint8_t BMMessage::getPayloadSize()
{
  return buffer[PAYLOAD_SIZE_INDEX];
};

void BMMessage::setPayload(const uint8_t *newPayload, uint8_t size)
{
  buffer[PAYLOAD_SIZE_INDEX] = size;
  memcpy(buffer+PAYLOAD_INDEX, newPayload, size);
};

uint16_t BMMessage::getCRC()
{
  // Get the crc value that is behind the payload
  uint16_t crcIndex = PAYLOAD_INDEX + getPayloadSize();
  return ((uint16_t)buffer[crcIndex] << 8) | ((uint16_t) buffer[crcIndex + 1]);
}

void BMMessage::setCRC(uint16_t crc)
{
  uint16_t crcIndex = PAYLOAD_INDEX + getPayloadSize();
  buffer[crcIndex]    = (uint8_t) (crc >> 8);
  buffer[crcIndex+1]  = (uint8_t) crc;
}

uint16_t BMMessage::computeCRC()
{
  return crc16(buffer, getSizeWithoutCRC());
}

bool BMMessage::isCRCValid()
{
  return getCRC() == computeCRC();
}

uint16_t BMMessage::getSize()
{
  return 4 + getPayloadSize();
}

uint16_t BMMessage::getSizeWithoutCRC()
{
  return 2 + getPayloadSize();
}