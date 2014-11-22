#ifndef COMMAND_INTERPRETER_H_
#define COMMAND_INTERPRETER_H_

#include <stdint.h>
#include "FileManager.h"
#include "SDCard.h"

namespace BottleMail {
class CommandInterpreter
{
  public:
  CommandInterpreter(SDCard &sdCard, FileManager &fileManager) : sdCard(sdCard), fileManager(fileManager) {};  
  
  inline void processByte(const uint8_t byte)
  {
    storeByte(byte);
    
    if (isCommandReceived())
      interpreteCommand();

    if (isExecuteToken(byte))
      idx = 0;
  }
  
private:

/**
  * Detect if a ASCII command was completely received
  *   Valid commands may be e.g.:
  *   '\n'
  *   'l' , \n'
  *   'r' , '1', '0', '\n'
  * @return True if command is detected, else otherwise.
  */
inline bool isCommandReceived()
{
  return (idx > 1 && commandBuffer[idx - 1] == '\n');
}

/**
 * Transforms the command buffer to a string by replacing the execute sign with \0.
 */
inline void transformCommandBufferToString()
{
  commandBuffer[idx - 1] = 0;
}

/**
 * Parses the value from the command buffer.
 * @param [out] value The parsed  value from the command buffer.
 * @return True if a value was found, else otherwise
 */
inline bool getValue(uint16_t &value)
{
  if (idx > 2)
  {
    value = atol((const char *)commandBuffer + 1);
    return true;
  }

  return false;
}

  inline void storeByte(uint8_t byte)
  {
    if (idx >= sizeof(commandBuffer))
      idx = 0;

    commandBuffer[idx++] = byte;
  }

inline bool isExecuteToken(const uint8_t byte)
{
  return (byte == '\n');
}

void interpreteCommand()
{
  transformCommandBufferToString();
  uint8_t cmd = commandBuffer[0];
  uint16_t value = 0;
  bool hasValue = getValue(value);

  if (cmd == 'l')
  {
    // listRoot();
  }

  else if (cmd == 'r' && hasValue)
  {
    /*
    Serial.print("Reading message : ");
    Serial.println(value);
    */

    FileManager::error_t err;
    err = FileManager::readMessage(value);
    Serial.println(err);
  }

  else if (cmd == 'w' && hasValue)
  {
    /*
     Serial.print("Writing message : ");
     Serial.println(value);
     */

    FileManager::error_t err;
    err = FileManager::writeMessage(value);
    Serial.println(err);
  }

  else if (cmd == 'm')
  {
    Serial.println(FileManager::getMessageCount());
  }
}
 
  private:
  SDCard &sdCard;
  FileManager &fileManager;
  
  // Command Buffer
  unsigned idx = 0;
  uint8_t commandBuffer[8];
};

}
#endif

