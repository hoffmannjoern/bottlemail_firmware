
#include <SdFile.h>
#include <SdFat.h>

#include "SDCard.h"
#include "FileManager.h"
#include "CommandInterpreter.h"
// #include "BLEModule.h"

using namespace BottleMail;

static SDCard sdcard(SS);
static FileManager fileManager;
static CommandInterpreter interpreter(sdcard, fileManager);

uint8_t receiveByte()
{
  // Wait until byte is received
  while (!Serial.available())
    ;

  return Serial.read();
}

void setupSerial()
{
  /*
  bool initialized = true;
  BLEModule ble(Serial);

  // Set com port speed to default speed
  Serial.begin(9600);
  delay(100);

  // Check if we can communicate with a BLE module in initial state
  if (ble.isResponding())
    initialized = ble.initialize();

  // Module is set up, set new transfer speed
  if (initialized)
    Serial.begin(ble.getBaud());
    */

  Serial.begin(38400);
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



