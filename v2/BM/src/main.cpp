#include <iostream>
#include "crc.hpp"

#include "BMMessage.hpp"
#include "Buffer.hpp"

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
}

int main(int argc, char *argv[])
{
  const uint16_t bufferSize = 64;
  uint8_t buffer [bufferSize] = {'a', 6, 'H', 'e', 'l', 'l', 'o', 0, 0xe4, 0x57};
  Buffer ibuf((char*)buffer, bufferSize);

  BMMessage msg1 = BMMessage(ibuf);
  printMsg(msg1);
  
  int err = msg1.writePayload(0, 0);
  if (err) {
    printf("Error writing payload!");
  }
  printMsg(msg1);
  
  BMMessage msg2 = BMMessage(ibuf);
  msg2.setCmd('m');
  msg2.setCRC();
  printMsg(msg2);
  
  msg2.appendPayload('h');
  msg2.appendPayload('\0');
  printMsg(msg2);

  //msg1.setCRC(crc);
  /*
	BMMessage msg1(buffer, bufferSize);
  
  printf("Command %c\n", msg1.getCmd());
  printf("Size payload %d\n", msg1.getPayloadSize());
  printf("Size wo CRC %d\n", msg1.getSizeWithoutCRC());
  printf("Size %d\n", msg1.getSize());
  
  uint16_t crc = msg1.computeCRC();
  printf("Compute CRC %x\n", crc);
  //msg1.setCRC(crc);

  printf("Compute CRC %x\n", msg1.getCRC());
  printf("Is crc valid ? %d\n", msg1.isCRCValid());
  
  for (int i = 0; i < msg1.getSize(); i++) {
    printf("%.2x ", buffer[i]);
  }
  
  printf("\n");
  */
}
