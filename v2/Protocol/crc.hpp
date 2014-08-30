#ifndef __CRC_H__
#define __CRC_H__

#include <stdio.h>

/// Computes CRC16 CCITT with 0xFFFF as initial polynom
uint16_t crc16_ccitt(const void *data, uint8_t length);

/// Computes crc32 with the standard polynom
uint32_t crc32(const void *data, size_t length, uint32_t previous_crc32 = 0);

#endif