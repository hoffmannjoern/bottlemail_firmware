#ifndef __CRC_H__
#define __CRC_H__

#include <stdio.h>

/**
 * Computes CRC16 CCITT checksum.
 * @note The value 0xFFFF as initial polynome is used.
 *
 * @param data Data from which the checksum is generated.
 * @param length The length of the data in bytes.
 * @return The crc16 checksum (short value).
 */
uint16_t crc16_ccitt(const void *data, uint8_t length);

/**
 * Computes CRC32 checksum
 * @note The standard polynome 0xEDB88320 is used.
 *
 * @param data Data from which the checksum is generated.
 * @param length The length of the data in bytes.
 * @param previous_crc32 The initial value of the polynom.
 * @return The crc32 checksum.
 */
uint32_t crc32(const void *data, size_t length, uint32_t previous_crc32 = 0);

#endif