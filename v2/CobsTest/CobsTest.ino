/*
 * StuffData byte stuffs "length" bytes of
 * data at the location pointed to by "ptr",
 * writing the output to the location pointed
 * to by "dst".
 */
 
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


void setup() 
{ 
  Serial.begin(57600);
}

typedef struct 
{
  uint8_t  cmd;
  uint32_t val;
}BMMessage;

#define RECV_COUNT_MAX 9
static uint8_t currentIndex = 0;
static uint8_t recvIndex    = 0;
static uint8_t recvBuffer[RECV_COUNT_MAX];

void recvByte(uint8_t aByte)
{
  // Store byte
  recvBuffer[recvIndex] = aByte;
  currentIndex = recvIndex;
  
  // Check if receive index exceed maximum
  recvIndex++;
  if (recvIndex >= RECV_COUNT_MAX)
    recvIndex = 0;    
}

void recvBufferReset()
{
  recvBuffer[0] = 0;
  currentIndex = recvIndex = 0;
}

void recvBufferDecode(uint8_t cobsIndex)
{
  if (cobsIndex < 1)
    return;
  
  int err = cobsDecode(recvBuffer, cobsIndex, recvBuffer);
  if (!err)
  {
    // Print decoded stuff
    for(unsigned i = 0; i < cobsIndex; i++)
      Serial.print((char)recvBuffer[i]);
  }
  
  else
    Serial.print("Error");
  
}

void loop() 
{ 
  if(Serial.available()) {
    recvByte(Serial.read());
  }
  
  if (recvBuffer[currentIndex] == 0x00)
  {
    // If we have characters, decode buffer with cobs.
    if (currentIndex > 1)
      recvBufferDecode(currentIndex-1);
      
   recvBufferReset();
  }
}
