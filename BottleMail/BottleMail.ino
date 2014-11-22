
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

void setupSerial()
{
  BLEModule ble(Serial);
  bool initialized = true;

  // Set default serial speed
  Serial.begin(9600);
  delay(100);

  // Check if the BLE module is responding at initial speed.
  // Thus, it is not configured.
  if (ble.isResponding())
    initialized = ble.configure();

  // Module is set up, set new transfer speed
  if (initialized)
    Serial.begin(ble.getBaud());
}

void setup(void)
{
  setupSerial();

  sdcard.initialize();
  fileManager.initialize();

  Serial.println();
  Serial.println(F("Type any character to start"));
}

void loop(void)
{
  uint8_t byte = receiveByte();

  // ASCII interpreter
  interpreter.processByte(byte);
}



