#include "protocol.hpp"
#include "crc.hpp"
#include <string.h>


BMMessage::BMMessage(const void *buffer)
{
	header 	= (BMMessageHeader_t *)buffer;
	payload	= (unsigned char*)buffer + sizeof(BMMessageHeader_t);
}

BMMessage::BMMessage(void *buffer, uint8_t command, uint8_t payloadLength, const void *payload)
{
	// Construct header
	BMMessageHeader_t *aHeader = (BMMessageHeader_t*) buffer;
	aHeader->command = command;
	aHeader->length  = payloadLength;
	aHeader->crc16	 = crc16(payload, payloadLength);
	this->header = aHeader;
	
	// Copy payload
	this->payload = (unsigned char*)buffer + sizeof(BMMessageHeader_t);
	memcpy(this->payload, payload, payloadLength);
}

bool BMMessage::isChecksumValid()
{
	if (header->length == 0)
	  return true;
	
	uint16_t checksum = crc16(this->payload, this->header->length); 
	return checksum == header->crc16;
}
