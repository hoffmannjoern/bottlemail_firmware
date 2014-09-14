/**
 * Encodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param ptr The pointer to the location to encode.
 * @param length The length of the location to encode.
 * @param dst The pointer to the location to write the encoded bytes. One more byte than the given length is needed.
 */
void cobsEncode(const unsigned char *ptr, unsigned long length, unsigned char *dst);

/**
 * Decodes a given location with the Constanly Overhead Byte Stuffing (COBS) algorithm.
 * @param ptr The pointer to the location to decode.
 * @param length The length of the location to decode.
 * @param dst The pointer to the location to write the encoded bytes. One byte less than length is needed.
 * @return Returns 0 on success, -1 otherwise.
 */
int cobsDecode(const unsigned char *ptr, unsigned long length, unsigned char *dst);