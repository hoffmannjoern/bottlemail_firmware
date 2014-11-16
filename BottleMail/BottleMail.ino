/*
 * This sketch reads and prints the file
 * PRINT00.TXT created by fat16print.ino or
 * WRITE00.TXT created by fat16write.ino
 */
#include <Fat16.h>
#include <Fat16util.h> // use functions to print strings from flash memory


#include "Command.h"
#include "CommandHandler.h"
#include "Frame.h"
#include "FrameRecognizer.h"
#include "FrameSender.h"
#include "FileManager.h"

const uint8_t CHIP_SELECT = SS;  // SD card chip select pin.
SdCard card;

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


void setup(void)
{
  Serial.begin(38400);

  // Initialize the SD card
  if (!card.begin(CHIP_SELECT))
    error("card.begin");

  // Initialize a FAT16 volume
  if (!Fat16::init(&card))
    error("Fat16::init");
}



using namespace BottleMail;
static CommandHandler commandHandler;
static FrameRecognizer frameRecognizer(commandHandler);

uint8_t receiveByte()
{
  // Wait until byte is received
  while (!Serial.available())
    ;

  return Serial.read();
}



void loop(void)
{
  Serial.println();
  Serial.println(F("Type any character to start"));

  uint8_t byte = receiveByte();

  frameRecognizer.addByte(byte);
  if (frameRecognizer.isProcessing())
    return;

  else if (byte == 'l')
  {
    // Show files
    Serial.println(F("Name          Modify Date/Time    Size"));
    Fat16::ls(LS_DATE | LS_SIZE);
    Serial.println(F("Done"));
  }

  else if (byte == 'r')
  {
    FileManager::initialize();
    Serial.println(FileManager::getMessageCount());
  }


  else if (byte == 'w')
    ;

}



