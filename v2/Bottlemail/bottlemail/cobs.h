#ifndef __COBS_H__
#define __COBS_H__

/**
 * Encodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param ptr The pointer to the location to encode.
 * @param length The length of the location to encode.
 * @param dst The pointer to the location to write the encoded bytes.
 * @param size The size of the destination location in byte. One byte more than the given length is needed to encode.
 * @return Returns 1 on success, 0 otherwise.
 */
int cobsEncode(const unsigned char *ptr, unsigned long length, unsigned char *dst, unsigned long size);

/**
 * Decodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param ptr The pointer to the location to decode.
 * @param length The length of the location to decode.
 * @param dst The pointer to the location to write the encoded bytes.
 * @param size The size of the destination location in byte. One byte less than length is needed to decode.
 * @return Returns 1 on success, 0 otherwise.
 */
int cobsDecode(const unsigned char *ptr, unsigned long length, unsigned char *dst, unsigned long size);

#endif