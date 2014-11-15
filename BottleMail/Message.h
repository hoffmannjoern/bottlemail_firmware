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
    /**
        * Supported commands of the bottle mail firmware
        */
    enum cmd_t : uint8_t
    {
      kCmdNone = 0,
      kCmdNotSupported = 1,

      kCmdAskMessageCount = 'm',
      kCmdAnswerMessageCount = 'n',

      kCmdReadMessage = 'r',
      kCmdReadMessageReady,

      kCmdWriteMessage = 'w',
      kCmdWriteMessageReady,
    };

    cmd_t cmd;
    uint16_t value;
} __attribute__((packed));

}
#endif /* MESSAGE_H_ */