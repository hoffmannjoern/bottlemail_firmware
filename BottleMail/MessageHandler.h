/*
 * MessageHandler.h
 *
 * Created: 14.11.2014 17:39:24
 *  Author: joern
 */


#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include "Message.h"
#include "FrameSender.h"
namespace BottleMail {

class MessageHandler
{
  public:
    inline void handle(const Message &message) const
    {
      Message msg;

      switch (message.cmd)
      {
        case Message::kCmdWriteMessage:
          msg.cmd = Message::kCmdWriteMessageReady;
          break;

        case Message::kCmdReadMessage :
          msg.cmd = Message::kCmdReadMessageReady;
          break;

        case Message::kCmdAskMessageCount :
          msg.cmd = Message::kCmdAnswerMessageCount;
          msg.value = 1;
          break;

        default:
          msg.cmd = Message::kCmdNotSupported;
      }

      FrameSender::sendMessage(Serial, msg);
    };
};
}
#endif /* MESSAGEHANDLER_H_ */