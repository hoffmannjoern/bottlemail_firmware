/*
 * Message.h
 *
 * Created: 14.11.2014 15:51:24
 *  Author: joern
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "endian.h"

namespace BottleMail {
class Message
{
  public:
    uint8_t cmd;
    uint16_t value;

  private:
} __attribute__((packed));

}
#endif /* MESSAGE_H_ */