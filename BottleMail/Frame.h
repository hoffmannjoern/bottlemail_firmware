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

class Frame
{
  public:
    enum frame_marker_t : uint8_t
    {
      SOF   = 'S',  ///< Start of a Bottl(e)Mail message frame.
    };

  public:
    /**
        * Initializes a frame with an empty message
        */
    Frame() : sof(SOF) {};

    /**
        * Initializes the frame with the given message.
        * @param message The message to be contained in the frame.
        */
    Frame(const Message &message) : Frame()
    {
      setMessage(message);
    }

    /**
        * Sets the frame message.
        * @note The byte order of the message will be corrected to network order and the checksum is updated when setting a message.
        * @param message Message to be contained in a BottleMail frame.
        */
    inline void setMessage(const Message &message)
    {
      msg.cmd = message.cmd;
      msg.value = HTONS(message.value);
      updateChecksum();
    }

    /**
        * Returns the containing message.
        * @note Before using the method check with isValid().
        * @return The message that is contained in the string.
        */
    inline Message getMessage() const
    {
      Message message;
      message.cmd = msg.cmd;
      message.value = NTOHS(msg.value);

      return message;
    }

    /**
        * Returns if the frame and it's containing data is valid.
        * @return True if the frame data is valid, false otherwise.
        */
    bool isValid() const
    {
      return sof == SOF && calculateChecksum() == crc8;
    }

    /**
        * Index operator to access the bytes of the frame from the beginning.
        * @note No boundary check is made.
        * @return The underlying byte of this frame. The first byte is the SOF indicator and the last is the checksum.
        */
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