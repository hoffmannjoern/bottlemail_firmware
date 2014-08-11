#include <iostream>
#include "crc.hpp"
#include "protocol.hpp"

using namespace std;
int main(int argc, char *argv[])
{
	
	char buffer [64]; // = { 'a', 5, 0xda, 0xda, 'H', 'e', 'l', 'l', 'o'};
	char payload[] = "Hello";
	BMMessage msg1(buffer, 'a', sizeof(payload), payload);
	
	BMMessageHeader_t *header = (BMMessageHeader_t*) buffer;
	printf("command : %c \n", header->command);
	printf("length  : %d \n", header->length);
	printf("crc16   : %x \n", header->crc16);
	
	BMMessage msg2(buffer);
	printf("message valid %s \n", msg2.isChecksumValid() ? "true" : "false");

	

	//printf("crc16 of %s (%lu) : %x \n", buffer, strlen(buffer), crc16(buffer, strlen(buffer)));
	//printf("crc32 of %s (%lu) : %x \n", buffer, strlen(buffer), crc32(buffer, strlen(buffer)));
}