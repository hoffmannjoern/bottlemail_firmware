
#include <SdFile.h>
#include <SdFat.h>

#include "SDCard.h"
#include "FileManager.h"
#include "CommandInterpreter.h"
#include "BLEModule.h"

using namespace BottleMail;

static SDCard sdcard(SS);
static FileManager fileManager;
static CommandInterpreter interpreter(sdcard, fileManager);

uint8_t receiveByte()
{
  while (!Serial.available())
    ;

  return Serial.read();
}

void setup(void)
{
  BLEModule ble(Serial);

  // Setup the module
  ble.setup();
  sdcard.initialize();
  fileManager.initialize();

  Serial.println(F(" **** BOTTL(E)MAIL ARDUINO V1 ****\n\n 2K RAM SYSTEM  1024 C BYTES FREE\n\nREADY."));
}

void loop(void)
{
  uint8_t byte = receiveByte();

  // ASCII interpreter
  interpreter.processByte(byte);
}



