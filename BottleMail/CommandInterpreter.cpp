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

  if (cmd == kCommandRead && hasValue)
    readWriteMessage(value, false);

  else if (cmd == kCommandWrite && hasValue)
    readWriteMessage(value, true);

  else if (cmd == kCommandAppend)
    readWriteMessage(fileManager.getCount(), true);

  else if (cmd == kCommandMessageCount)
    answer(fileManager.getCount());

  else if (cmd == kCommandList)
    sdCard.listRootDir();
}


void CommandInterpreter::readWriteMessage(const uint16_t &messageNumber, const bool write)
{
  // Select
  FileManager::error_t error;
  error = fileManager.select(messageNumber, write);
  answer(error);
  if (error)
    return;

  // Perform operation
  if (write)
    error = fileManager.write(messageNumber);
  else
    error = FileManager::read(messageNumber);

  answer(error);
}

}