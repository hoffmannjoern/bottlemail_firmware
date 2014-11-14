/*
 * Frame.h
 *
 * Created: 14.11.2014 17:05:00
 *  Author: joern
 */


#ifndef FRAME_H_
#define FRAME_H_

#include "Message.h"

extern "C" {
#include <avr/crc16.h>
};

namespace BottleMail {

/**
 *  Start of a Bottl(e)Mail message frame.
 */
const uint8_t SOF = 'S';

class Frame
{
  public:
    Frame() : sof(SOF) {};

    inline void setMessage(Message &message)
    {
      msg.cmd = message.cmd;
      msg.value = HTONS(message.value);

      updateChecksum();
    }

    inline Message getMessage() const
    {
      Message message;
      message.cmd = msg.cmd;
      message.value = NTOHS(msg.value);

      return message;
    }

    bool isValid() const
    {
      return sof == SOF && calculateChecksum() == crc8;
    }

    uint8_t &operator[] (const int nIndex)
    {
      return ((uint8_t *) &sof)[nIndex];
    }

  private:
    inline void updateChecksum()
    {
      crc8 = calculateChecksum();
    }

    inline uint8_t calculateChecksum() const
    {
      register uint8_t checksum = 0;
      const uint8_t *ptr = (const uint8_t *) &msg;

      for (int i = 0; i < sizeof(Message); i++)
        checksum = _crc8_ccitt_update(checksum, ptr[i]);

      return checksum;
    }

  private:
    uint8_t sof;
    Message msg;
    uint8_t crc8;

} __attribute__((packed));

}

#endif /* FRAME_H_ */