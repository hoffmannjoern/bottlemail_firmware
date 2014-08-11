#include <stdio.h>
#include <stdint.h>

static const uint32_t crc32_poly = 0xEDB88320;

uint16_t crc16(const void *data, uint8_t length)
{
    uint8_t x;
	uint16_t crc = 0xFFFF;
	unsigned char *current = (unsigned char *)data;

    while (length--)
	{
        x = crc >> 8 ^ *current++;
        x ^= x>>4;
        crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
    }

    return crc;
}

uint32_t crc32(const void *data, size_t length, uint32_t previousCrc32 = 0)
{
  uint32_t crc = ~previousCrc32;
  unsigned char *current = (unsigned char*) data;

  while (length--)
  {
    crc ^= *current++;
    for (unsigned int j = 0; j < 8; j++)
      crc = (crc >> 1) ^ (-int(crc & 1) & crc32_poly);
  }

  return ~crc; // same as crc ^ 0xFFFFFFFF
}