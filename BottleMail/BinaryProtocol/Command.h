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

class Command
{
  public:
    /**
        * Supported command codes of the bottle mail firmware.
        */
    enum cmd_t : uint8_t
    {
      kCmdNone                = 0x00,
      kCmdNotSupported,

      // Count
      kCmdMessageCountAsk     = 0x10,
      kCmdMessageCountAnswer,

      // Read
      kCmdReadMessage         = 0x20,
      kCmdReadMessageAnswer,

      // Write
      kCmdWriteMessage        = 0x30,
      kCmdWriteMessageAnswer,
    };

    /**
        * Error codes that will be returned on commands as the value field.
        */
    enum error_t : uint16_t
    {
      kErrorNone = 0x00,

      // Message Errors
      kErrorMessageNumberInvalid = 0x10,
      kErrorMessageFileNotFound,
    };

    /**
        * Creates a new command with given type and value.
        * @param cmd The command.
        * @param value The argument of the command.
        */
    Command(cmd_t cmd = kCmdNone, uint16_t value = 0) : cmd(cmd), value(value) {};

    /**
        * The code of the command
        */
    cmd_t cmd;

    /**
        * The value or argument of the command
        */
    uint16_t value;
} __attribute__((packed));

}
#endif /* MESSAGE_H_ */