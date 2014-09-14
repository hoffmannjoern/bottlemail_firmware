#include <stdlib.h>
#include <stdint.h>


size_t cobs_encode(void *v_output, size_t output_size, const void *v_input, size_t length)
{
  const unsigned char *input = v_input;
  unsigned char *output = v_output;
  
  if (!length)
    return 0;
  
  else if (input == NULL || output == NULL || (length+1 > output_size))
    return 0;
  
  uint8_t code        = 1;
  size_t  code_index  = 0;
  size_t  read_index  = 0;
  size_t  write_index = 1;
  uint8_t lastByte = 0;
  
  while(read_index < length)
  {
    uint8_t byte = input[read_index++];
    
    if(byte == 0)
    {
      output[code_index] = code;
      code_index = write_index++;
      code = 1;
    }
    
    else
    {
      output[write_index++] = byte;
      code++;
      
      if(code == 0xFF)
      {
        output[code_index] = code;
        code_index = write_index++;
        code = 1;
      }
    }
  }
  
  output[code_index] = code;
  
  return write_index;
}

int cobs_encode_inplace(void *v_output, unsigned long output_size, const void *v_input, unsigned long length)
{
  const unsigned char *input = v_input;
  unsigned char *output = v_output;
  
  if (!length)
    return 1;
  
  else if (input == NULL || output == NULL || (length+1 > output_size))
    return 0;
  
  // The code and code pointer to define the amount of following characters
  unsigned char code = 0x01;
  unsigned char *code_ptr = output++;
  unsigned char lastByte = 0;
  
  // Blockwise stuff bytes to the destination
  const unsigned char *end = input + length;
  while (input < end)
  {
    // Read current byte
    unsigned char byte = *input;
    
    // Write the last read byte. Note it can't be 0.
    if (lastByte) {
      *output++ = lastByte;
      lastByte = 0;
    }
    
    // If we detect a zero byte, finish the current code.
    if (byte == 0)
    {
      *code_ptr = code;
      code_ptr  = output++;
      code = 1;
    }
    
    // Otherwise process this byte.
    else
    {
      // Save byte and increment the code (byte count)
      lastByte = byte;
      code++;
      
      // If we reach the maximum byte count (UCHAR_MAX), write down the code to prevent an overflow.
      if (code == 0xFF)
      {
        // Fixme:
        // Bug for packages >= 255;
        
        // Save next byte and write down current byte;
        lastByte = *(input+1);
        *output++ = byte;
        
        // Finish block
        *code_ptr = code;
        code_ptr  = output++;
        code = 1;
        
        if (lastByte == 0)
        {
          *code_ptr = code;
          code_ptr  = output++;
          code = 1;
        }
      }
    }
    
    // Reference next byte
    input++;
  }
  
  if (lastByte)
    *output++ = lastByte;
  
  // Write down last block count.
  *code_ptr = code;
  
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