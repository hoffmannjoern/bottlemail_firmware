/*
 * StuffData byte stuffs "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst".
 */

void cobsEncode(const unsigned char *ptr, unsigned long length, unsigned char *dst);

/*
 * UnStuffData decodes "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst".
 *
 * Returns 0 if successful, otherwise -1.
 */
int cobsDecode(const unsigned char *ptr, unsigned long length, unsigned char *dst);