#ifndef __COBS_H__
#define __COBS_H__

// FIXME: Larger packages.
// Functions only support packages with <= 254 byte length.

/**
 * Encodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param dst The pointer to the location to write the encoded bytes.
 * @param dst_size The size of the destination location in byte. One byte more than the given length is needed to encode.
 * @param ptr The pointer to the location to encode.
 * @param length The length of the location to encode.
 * @return Returns 1 on success, 0 otherwise.
 */
int cobs_encode(void *dst, unsigned long dst_size, const void *ptr, unsigned long length);

/**
 * Decodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param dst The pointer to the location to write the encoded bytes.
 * @param dst_size The size of the destination location in byte. One byte less than length is needed to decode.
 * @param ptr The pointer to the location to decode.
 * @param length The length of the location to decode.
 * @return Returns 1 on success, 0 otherwise.
 */
int cobs_decode(void *dst, unsigned long dst_size, const void *ptr, unsigned long length);

#endif