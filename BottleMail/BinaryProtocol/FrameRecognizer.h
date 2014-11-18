/*
 * FrameRecognizer.h
 *
 * Created: 15.11.2014 09:11:05
 *  Author: jhoffmann
 */


#ifndef FRAMERECOGNIZER_H_
#define FRAMERECOGNIZER_H_

#include "Frame.h"
#include "CommandHandler.h"

namespace BottleMail {
class FrameRecognizer
{
  public:
    FrameRecognizer(CommandHandler &commandHandler) :
      _commandHandler(commandHandler)
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
      _byteIndex = 0;
    }

    inline bool isProcessing() const
    {
      return _byteIndex > 0;
    }

  private:
    inline void startProcessing()
    {
      reset();
      addByteToFrame(Frame::SOF);
    }

    inline bool isFrameCompleted() const
    {
      return _byteIndex >= sizeof(Frame);
    }

    inline void addByteToFrame(uint8_t byte)
    {
      if (_byteIndex < sizeof(Frame))
        _frame[_byteIndex++] = byte;
    }

    inline void processFrame() const
    {
      if (_frame.isValid())
      {
        Command command = _frame.getCommand();
        _commandHandler.handle(command);
      }
    }


  private:
    Frame _frame;
    uint8_t _byteIndex;
    CommandHandler &_commandHandler;
};

}
#endif /* FRAMEHANDLER_H_ */