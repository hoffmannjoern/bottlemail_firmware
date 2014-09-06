/*
 * StuffData byte stuffs "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst".
 */

#define NULL 0

#define FinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)

void cobsEncode(const unsigned char *ptr, unsigned long length, unsigned char *dst)
{
  if (ptr == NULL || dst == NULL)
    return;
	
  const unsigned char *end = ptr + length;
  unsigned char *code_ptr = dst++;
  unsigned char code = 0x01;
  
  while (ptr < end)
  {
    if (*ptr == 0)
      FinishBlock(code);
    else
    {
      *dst++ = *ptr;
      code++;
      if (code == 0xFF)
        FinishBlock(code);
    }
    ptr++;
  }
  
  FinishBlock(code);
}

/*
 * UnStuffData decodes "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst".
 */
int cobsDecode(const unsigned char *ptr, unsigned long length, unsigned char *dst)
{
  if (ptr == NULL || dst == NULL)
    return 0;
	
  const unsigned char *ptrEnd = ptr + length;
  while (ptr < ptrEnd)
  {
    int code = *ptr;
    if (ptr + code -1 > ptrEnd)
      return -1;
    
    ptr++;
    for (int i=1; i<code; i++)
      *dst++ = *ptr++;
    
    if (code < 0xFF)
      *dst++ = 0;
  }
  
  return 0;
}