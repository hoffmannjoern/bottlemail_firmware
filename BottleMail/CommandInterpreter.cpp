/*
 * CommandInterpreter.cpp
 *
 * Created: 22.11.2014 21:19:16
 *  Author: joern
 */

#include "CommandInterpreter.h"
namespace BottleMail {


void CommandInterpreter::processByte(const uint8_t byte)
{
  storeByte(byte);

  if (detectCommand())
    interpreteCommand();

  if (isCommandToken(byte))
    idx = 0;
}

void CommandInterpreter::interpreteCommand()
{
  transformCommandBufferToString();
  uint8_t cmd = commandBuffer[0];
  uint16_t value = 0;
  bool hasValue = getValue(value);

  if (cmd == 'l')
    sdCard.listRootDir();

  else if (cmd == 'r' && hasValue)
  {
    FileManager::error_t err;
    err = FileManager::readMessage(value);
    Serial.println(err);
  }

  else if (cmd == 'w' && hasValue)
  {
    FileManager::error_t err;
    err = FileManager::writeMessage(value);
    Serial.println(err);
  }

  else if (cmd == 'm')
  {
    Serial.println(FileManager::getMessageCount());
  }
}

}