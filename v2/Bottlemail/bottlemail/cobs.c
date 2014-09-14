#include <stdlib.h>
#include <stdint.h>

// Write down the code, adjust the code ptr and set a new code.
#define FinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)

int cobs_encode(void *vDst, unsigned long dst_size, const void *vPtr, unsigned long length)
{
  const unsigned char *ptr = vPtr;
  unsigned char *dst = vDst;
  
  if (!length)
    return 1;
  
  else if (ptr == NULL || dst == NULL || (length+1 > dst_size))
    return 0;
  
  // The code and code pointer to define the amount of following characters
  unsigned char code = 0x01;
  unsigned char *code_ptr = dst++;
  unsigned char byte, lastByte = 0;
  
  // Blockwise stuff bytes to the destination
  const unsigned char *end = ptr + length;
  while (ptr < end)
  {
    // Read current byte
    byte = *ptr;
    
    // Write the last read byte. Note it can't be 0.
    if (lastByte) {
      *dst++ = lastByte;
      lastByte = 0;
    }
    
    // If we detect a zero byte, finish the current code.
    if (byte == 0)
      FinishBlock(code);
    
    // Otherwise process this byte.
    else
    {
      // Save byte and increment the code (byte count)
      lastByte = byte;
      code++;
      
      // If we reach the maximum byte count (UCHAR_MAX), write down the code to prevent an overflow.
      if (code == 0xFF)
      {
        
        // Save next byte and write down current byte;
        lastByte = *(ptr+1);
        *dst++ = byte;
        FinishBlock(code);
        
        if (lastByte == 0) {
          FinishBlock(code);
        }
      }
    }
    
    // Reference next byte
    ptr++;
  }
  
  if (lastByte)
    *dst++ = lastByte;
  
  // Write down last block.
  FinishBlock(code);
  return 1;
}

int cobs_decode(void *vDst, unsigned long dst_size, const void *vPtr, unsigned long length)
{
  const unsigned char *ptr = vPtr;
  unsigned char *dst = vDst;
  
  if (!length)
    return 1;
  
  else if (ptr == NULL || dst == NULL || (length > dst_size))
    return 0;
  
  // Blockwise unstuff bytes to the destination
  const unsigned char *end = ptr + length;
  while (ptr < end)
  {
    // Read and check the first byte of this block to determine the length of this block.
    unsigned char code = *ptr;
    if (ptr + code-1 > end)
      return 0;
    
    // Copy the payload bytes of this block to the destination.
    ptr++;
    for (unsigned char i=1; i<code; i++)
      *dst++ = *ptr++;
    
    // If the code is not UCHAR_MAX, we know that a zero have to be writte after the unstuffed bytes.
    if (code < 0xFF)
      *dst++ = 0;
  }
  
  return 1;
}