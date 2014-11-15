/*
 * FrameRecognizer.h
 *
 * Created: 15.11.2014 09:11:05
 *  Author: jhoffmann
 */


#ifndef FRAMERECOGNIZER_H_
#define FRAMERECOGNIZER_H_

#include "Frame.h"
#include "MessageHandler.h"

namespace BottleMail {
class FrameRecognizer
{
  public:
    FrameRecognizer(MessageHandler &messageHandler) :
      messageHandler(messageHandler)
    {
      reset();
    };

    void addByte(uint8_t byte)
    {
      if (!isProcessing())
      {
        if (byte == Frame::SOF)
          startProcessing();
      }

      else
      {
        addByteToFrame(byte);

        if (isFrameCompleted())
        {
          processFrame();
          reset();
        }
      }
    }

    inline void reset()
    {
      byteIndex = 0;
    }

    inline bool isProcessing() const
    {
      return byteIndex > 0;
    }

  private:
    inline void startProcessing()
    {
      reset();
      addByteToFrame(Frame::SOF);
    }

    inline bool isFrameCompleted() const
    {
      return byteIndex >= sizeof(Frame);
    }

    inline void addByteToFrame(uint8_t byte)
    {
      if (byteIndex < sizeof(Frame))
        frame[byteIndex++] = byte;
    }

    inline void processFrame() const
    {
      if (frame.isValid())
      {
        Message message = frame.getMessage();
        messageHandler.handle(message);
      }
    }


  private:
    Frame frame;
    uint8_t byteIndex;
    MessageHandler &messageHandler;
};

}
#endif /* FRAMEHANDLER_H_ */