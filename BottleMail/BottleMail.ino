#include "Fat16.h"
#include "Fat16util.h"

#include "Command.h"
#include "CommandHandler.h"
#include "Frame.h"
#include "FrameRecognizer.h"
#include "FrameSender.h"
#include "FileManager.h"

using namespace BottleMail;

static SdCard card;
static CommandHandler commandHandler;
static FrameRecognizer frameRecognizer(commandHandler);
const uint8_t CHIP_SELECT = SS;  // SD card chip select pin.


// TODO: refactor to file handler
// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char *str)
{
  Serial.print(F("error: "));
  SerialPrintln_P(str);
  if (card.errorCode)
  {
    Serial.print(F("SD error: "));
    Serial.println(card.errorCode, HEX);
  }
}

uint8_t receiveByte()
{
  // Wait until byte is received
  while (!Serial.available())
    ;

  return Serial.read();
}

void setup(void)
{
  Serial.begin(38400);

  // Initialize the SD card
  if (!card.begin(CHIP_SELECT))
    error("card.begin");

  // Initialize a FAT16 volume
  if (!Fat16::init(&card))
    error("Fat16::init");

  FileManager::initialize();

  Serial.println();
  Serial.println(F("Type any character to start"));
}

uint_fast8_t idx = 0;
uint8_t commandBuffer[8];

inline void storeByte(uint8_t byte)
{
  if (idx >= sizeof(commandBuffer))
    idx = 0;

  commandBuffer[idx++] = byte;
}

inline bool isExecuteChar(const uint8_t byte)
{
  return (byte == '\n');
}

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
inline void terminateCommandBuffer()
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

void interpreteCommand()
{
  terminateCommandBuffer();
  uint8_t cmd = commandBuffer[0];
  uint16_t value = 0;
  bool hasValue = getValue(value);

  if (cmd == 'l')
  {
    // Show files
    Serial.println(F("Name          Modify Date/Time    Size"));
    Fat16::ls(LS_DATE | LS_SIZE);
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


void loop(void)
{
  uint8_t byte = receiveByte();

  storeByte(byte);

  if (isCommandReceived())
    interpreteCommand();

  if (isExecuteChar(byte))
    idx = 0;

  // New protocol below:
  /*
  frameRecognizer.addByte(byte);
  if (frameRecognizer.isProcessing())
    return;
    */


}



