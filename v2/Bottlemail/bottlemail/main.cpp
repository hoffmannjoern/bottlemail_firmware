#include <iostream>
#include "crc.hpp"

#include "BMMessage.hpp"
#include "Buffer.hpp"
#include "StaticBuffer.hpp"
#include "DynamicBuffer.hpp"
#include "FillBuffer.hpp"
#include "CobsFrame.h"

using namespace std;

void printMsg(BMMessage &msg1)
{
  printf("Command %c\n", msg1.getCmd());
  Buffer payload = msg1.getPayloadBuffer();

  printf("Payload %s\n", payload.bytes);
  printf("Size payload  %d\n", payload.size);

  printf("Size w/o CRC  %d\n", msg1.getSizeWithoutCRC());
  printf("Size with CRC %d\n", msg1.getSize());
  
  printf("Setted CRC    %x\n", msg1.getCRC());
  printf("Computed CRC  %x\n", msg1.computeCRC());
  printf("Check CRC     %d\n", msg1.isCRCValid());
  printf("\n");
}

void test1(Buffer &buffer)
{
  BMMessage msg = BMMessage(buffer);
  
  printf("Using plain data as message:\n");
  printMsg(msg);
}

void test2(Buffer &buffer)
{
  BMMessage msg = BMMessage(buffer);
  
  printf("Deleted payload:\n");
  int err = msg.writePayload(0, 0);
  if (err)
    printf("Error writing payload!\n");
  
  printMsg(msg);
}

void test3(Buffer &buffer)
{
  
  BMMessage msg = BMMessage(buffer);
  
  printf("Set command 'm' and crc :\n");
  msg.setCmd('m');
  msg.setCRC();
  printMsg(msg);
  
  printf("Append payload 'h' :\n");
  printf("Crc should now be 0 :\n");
  msg.appendPayload('h');
  msg.appendPayload('\0');
  printMsg(msg);
}

void bufferTest()
{
  const uint16_t bufferSize = 64;
  uint8_t buffer [bufferSize] = {'a', 6, 'H', 'e', 'l', 'l', 'o', 0, 0xe4, 0x57};
  
  printf("Buffer Test:\n");
  Buffer bufferObj((char*)buffer, bufferSize);
  test1(bufferObj);
  test2(bufferObj);
  test3(bufferObj);
  
  printf("Dynamic Buffer Test:\n");
  DynamicBuffer buf(10);
  printf("Buffer pointer %p\n", buf.bytes);
  printf("Buffer size %d\n", buf.size);
  buf[0] = 'a';
  test3(buf);
  
  printf("Static Buffer Test:\n");
  StaticBuffer<20> staticBuffer;
  Buffer buf2 = staticBuffer;
  buf2[0] = 'H';
  
  printf("Buffer pointer %p\n", buf2.bytes);
  printf("Buffer pointer %p\n", ((StaticBuffer<20>*)&buf2)->bytes);
  printf("Buffer size %d\n", buf2.size);
  printf("Byte %c, %c\n\n", buf2[0], staticBuffer[0]);
  
  
  printf("Fill Buffer Test: \n");
  StaticBuffer<2> buhu;
  FillBuffer fillBuffer(buhu);
  
  printf("Buffer pointer %p\n", fillBuffer.bytes);
  printf("Buffer size %d\n", fillBuffer.size);
  printf("Buffer length %d\n", fillBuffer.getLength());
  printf("Buffer bytes %s\n", fillBuffer.bytes);
  
  /*
   for (int i = 0; i < 20; i++)
   {
   fillBuffer.appendByte('a' + i);
   }*/
  
  fillBuffer.appendByte('z');
  fillBuffer.appendByte(0);
  fillBuffer.appendByte(0);
  printf("Buffer length %d\n", fillBuffer.getLength());
  printf("Buffer bytes : %s\n", fillBuffer.bytes);
}

void cobsFrameTest()
{
  const int cBufferSize    = 10;
  uint8_t cBuffer[10]      = {0x61, 0x62, 0x63, 0x00, 0x64, 0x65, 'n', 'n', 'n'};
  uint8_t cBufferEncoded[] = {0x04, 0x61, 0x62, 0x63, 0x03, 0x64, 0x65};
  
  Buffer buffer(cBuffer, cBufferSize);
  CobsFrame frame(buffer, 6, false);
  printf("frame: %s\n", cBuffer);
  
  // Check encode
  bool ret = frame.encode();
  printf("length: %lu\n", frame.getLength());
  printf("encoded: %d\n", frame.isEncoded());
  
  ret = frame.decode();
  printf("frame %s\n", cBuffer);
}

int main(int argc, char *argv[])
{
  // bufferTest();
  
  cobsFrameTest();
  
}
