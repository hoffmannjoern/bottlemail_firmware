#ifndef __COBS_H__
#define __COBS_H__

// FIXME: Larger packages.
// Functions only support packages with <= 254 byte length.

/**
 * Encodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param output The pointer to the location to write the encoded bytes.
 * @param output_size The size of the destination location in byte. One byte more than the given length is needed to encode.
 * @param input The pointer to the location to encode.
 * @param length The length of the location to encode.
 * @return Returns the size of the encoded bytes.
 */
size_t cobs_encode(void *output, size_t output_size, const void *input, size_t length);

/**
 * Decodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param output The pointer to the location to write the encoded bytes.
 * @param output_size The size of the destination location in byte. One byte less than length is needed to decode.
 * @param input The pointer to the location to decode.
 * @param length The length of the location to decode.
 * @return Returns the size of the decoded bytes.
 */
size_t cobs_decode(void *output, size_t output_size, const void *input, size_t length);

#endif