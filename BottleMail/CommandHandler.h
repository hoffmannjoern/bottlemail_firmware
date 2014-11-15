/*
 * MessageHandler.h
 *
 * Created: 14.11.2014 17:39:24
 *  Author: joern
 */


#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_

#include "Command.h"
#include "FrameSender.h"
namespace BottleMail {

class CommandHandler
{
  public:
    inline void handle(const Command &command) const
    {
      Command answer;

      switch (command.cmd)
      {
        case Command::kCmdWriteMessage:
          answer.cmd = Command::kCmdWriteMessageReady;
          break;

        case Command::kCmdReadMessage :
          answer.cmd = Command::kCmdReadMessageReady;
          break;

        case Command::kCmdMessageCountAsk :
          answer.cmd = Command::kCmdMessageCountAnswer;
          answer.value = 1;
          break;

        default:
          answer.cmd = Command::kCmdNotSupported;
      }

      FrameSender::sendCommand(Serial, answer);
    };
};
}
#endif /* COMMANDHANDLER_H_ */