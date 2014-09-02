#include <iostream>
#include "crc.hpp"
#include "BMMessage.hpp"

using namespace std;
int main(int argc, char *argv[])
{
  const uint16_t bufferSize = 64;
	uint8_t buffer [bufferSize] = {'a', 6, 'H', 'e', 'l', 'l', 'o', 0, 0xe4, 0x57};
  BMMessage msg1 = BMMessage(buffer, 10);

  printf("Command %c\n", msg1.getCmd());
  printf("Payload %s\n", msg1.getPayload());
  printf("Size payload  %d\n", msg1.getPayloadSize());
  printf("Size w/o CRC  %d\n", msg1.getSizeWithoutCRC());
  printf("Size with CRC %d\n", msg1.getSize());
  
  printf("Setted CRC    %x\n", msg1.getCRC());
  printf("Computed CRC  %x\n", msg1.computeCRC());
  printf("Check CRC     %d\n", msg1.isCRCValid());
  
  uint8_t str[] = "Hellp";
  int err = msg1.writePayload(0, 0);
  if (err) {
    printf("Error writing payload!");
  }
  
  printf("Command %c\n", msg1.getCmd());
  printf("Payload %s\n", msg1.getPayload());
  printf("Size payload  %d\n", msg1.getPayloadSize());
  printf("Size w/o CRC  %d\n", msg1.getSizeWithoutCRC());
  printf("Size with CRC %d\n", msg1.getSize());
  
  printf("Setted CRC    %x\n", msg1.getCRC());
  printf("Computed CRC  %x\n", msg1.computeCRC());
  printf("Check CRC     %d\n", msg1.isCRCValid());

  
  
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