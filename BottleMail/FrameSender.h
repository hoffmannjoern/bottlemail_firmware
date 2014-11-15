/*
 * FrameSender.h
 *
 * Created: 15.11.2014 11:16:41
 *  Author: joern
 */


#ifndef FRAMESENDER_H_
#define FRAMESENDER_H_

#include <Stream.h>
#include "Frame.h"
#include "Message.h"

namespace BottleMail {

// TODO: add functionality for retry count etc.
class FrameSender
{
  public:
    static bool sendFrame(Stream &stream, Frame &frame)
    {
      for (int i = 0; i < sizeof(Frame); i++)
        stream.write(frame[i]);
    }

    static bool sendMessage(Stream &stream, Message &message)
    {
      Frame frame(message);
      return sendFrame(stream, frame);
    }
};

}

#endif /* FRAMESENDER_H_ */