/*
 * MessageHandler.h
 *
 * Created: 14.11.2014 17:39:24
 *  Author: joern
 */


#ifndef MESSAGEHANDLER_H_
#define MESSAGEHANDLER_H_

#include "Message.h"

namespace BottleMail {

class MessageHandler
{
  public:
    void handle(const Message &message) const
    {
      if (message.cmd == 'w')
        Serial.println("Handle w");

      else if (message.cmd == 'r')
        Serial.println("Handle r");
    }

};

}
#endif /* MESSAGEHANDLER_H_ */