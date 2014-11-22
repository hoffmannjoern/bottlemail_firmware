/*
 * CommandInterpreter.h
 *
 * Created: 22.11.2014
 *  Author: joern
 */

#ifndef COMMAND_INTERPRETER_H_
#define COMMAND_INTERPRETER_H_

#include <stdint.h>
#include "FileManager.h"
#include "SDCard.h"

namespace BottleMail {
class CommandInterpreter
{
  public:
    static const uint8_t commandToken = '\n';

    /**
        * Initializes the interpreter to process incoming bytes.
        * \param sdCard The sd card to process commands.
        * \param fileManager The file manager to process commands.
        */
    CommandInterpreter(SDCard &sdCard, FileManager &fileManager) : sdCard(sdCard), fileManager(fileManager) {};

    /** Processes a new byte
         * @param byte The byte to process, thus to append to the internal buffer.
         */
    void processByte(const uint8_t byte);

  private:
    inline void storeByte(const uint8_t &byte);
    inline bool detectCommand() const;
    inline bool isCommandToken(const uint8_t &byte) const;
    inline void transformCommandBufferToString();
    inline bool getValue(uint16_t &value) const;
    void interpreteCommand();

  private:
    SDCard &sdCard;
    FileManager &fileManager;

    // Command Buffer
    unsigned idx = 0;
    uint8_t commandBuffer[8];
};


/**
  * Stores the byte to the internal command buffer.
  */
inline void CommandInterpreter::storeByte(const uint8_t &byte)
{
  if (idx >= sizeof(commandBuffer))
    idx = 0;

  commandBuffer[idx++] = byte;
}

/**
  *  Returns true if the given byte value is equal to the command token
  */
inline bool CommandInterpreter::isCommandToken(const uint8_t &byte) const
{
  return (byte == commandToken);
}

/**
  * Detect if a ASCII command was completely received
  *   Valid commands may be e.g.:
  *   '\n'
  *   'l' , \n'
  *   'r' , '1', '0', '\n'
  * @return True if command is detected, else otherwise.
  */
inline bool CommandInterpreter::detectCommand() const
{
  return (idx > 1 && isCommandToken(commandBuffer[idx - 1]));
};


/**
  * Transforms the command buffer to a string by replacing the execute sign with \0.
  */
inline void CommandInterpreter::transformCommandBufferToString()
{
  commandBuffer[idx - 1] = 0;
}

/**
  * Parses the value from the command buffer.
  * @param [out] value The parsed  value from the command buffer.
  * @return True if a value was found, else otherwise
  */
inline bool CommandInterpreter::getValue(uint16_t &value) const
{
  if (idx > 2)
  {
    value = atol((const char *)commandBuffer + 1);
    return true;
  }

  return false;
}

}
#endif

