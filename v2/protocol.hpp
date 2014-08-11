#ifndef __BM_PROTO_HPP__
#define __BM_PROTO_HPP__

#include "stdint.h"

typedef struct
{
	uint8_t  command;
	uint8_t  length;
	uint16_t crc16;
} BMMessageHeader_t;


class BMMessage
{
  public:
  BMMessage(const void *buffer);
  BMMessage(void *buffer, uint8_t cmd, uint8_t payloadLength, const void *payload);

  bool isChecksumValid();

  private:
	BMMessageHeader_t *header;
	void *payload;
};

#endif