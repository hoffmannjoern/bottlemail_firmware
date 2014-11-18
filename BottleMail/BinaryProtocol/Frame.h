/*
 * Frame.h
 *
 * Created: 14.11.2014 17:05:00
 *  Author: joern
 */


#ifndef FRAME_H_
#define FRAME_H_

#include "Command.h"

extern "C" {
#include <avr/crc16.h>
};

namespace BottleMail {

class Frame
{
  public:
    enum frame_marker_t : uint8_t
    {
      SOF   = 'S',  ///< Start of a Bottl(e)Mail command frame.
    };

  public:
    /**
        * Initializes a frame with an empty command
        */
    Frame() : _sof(SOF) {};

    /**
        * Initializes the frame with the given command.
        * @param command The command to be contained in the frame.
        */
    Frame(const Command &command) : Frame()
    {
      setCommand(command);
    }

    /**
        * Sets the frame command.
        * @note The byte order of the command will be corrected to network order and the checksum is updated when setting a command.
        * @param command The command to be contained in a BottleMail frame.
        */
    inline void setCommand(const Command &command)
    {
      _command.cmd = command.cmd;
      _command.value = HTONS(command.value);
      updateChecksum();
    }

    /**
        * Returns the containing command.
        * @note Before using the method check with isValid().
        * @return The command that is contained in the string.
        */
    inline Command getCommand() const
    {
      Command command;
      command.cmd = _command.cmd;
      command.value = NTOHS(_command.value);

      return command;
    }

    /**
        * Returns if the frame and it's containing data is valid.
        * @return True if the frame data is valid, false otherwise.
        */
    bool isValid() const
    {
      return _sof == SOF && calculateChecksum() == _crc8;
    }

    /**
        * Index operator to access the bytes of the frame from the beginning.
        * @note No boundary check is made.
        * @return The underlying byte of this frame. The first byte is the SOF indicator and the last is the checksum.
        */
    uint8_t &operator[] (const int nIndex)
    {
      return ((uint8_t *) &_sof)[nIndex];
    }

  private:
    inline void updateChecksum()
    {
      _crc8 = calculateChecksum();
    }

    inline uint8_t calculateChecksum() const
    {
      register uint8_t checksum = 0;
      const uint8_t *ptr = (const uint8_t *) &_command;

      for (int i = 0; i < sizeof(Command); i++)
        checksum = _crc8_ccitt_update(checksum, ptr[i]);

      return checksum;
    }

  private:
    uint8_t _sof;
    Command _command;
    uint8_t _crc8;

} __attribute__((packed));

}

#endif /* FRAME_H_ */