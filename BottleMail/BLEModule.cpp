/*
 * BLEModule.cpp
 *
 * Created: 22.11.2014 21:39:10
 *  Author: joern
 */

#include "BLEModule.h"

namespace BottleMail {

bool BLEModule::isResponding() const
{
  com.print(F("AT"));

  // Read answer byte-by-byte
  // We expect only a "OK"
  // TODO: refactor this method
  char recv[2];
  for (int index = 0; index < 2; index++)
  {
    if (!com.available())
      delay(50);

    recv[index] = com.read();
  }

  // Return true if "OK" received
  return (recv[0] == 'O' && recv[1] == 'K');
}

bool BLEModule::configure()
{
  // Disable Power Management (curiously have to set to 1!)
  com.print(F("AT+PWRM1"));
  delay(50);

  // Set Name
  com.print(F("AT+NAMEBottleMail"));
  delay(50);

  // Disable connection saving
  com.print(F("ST+SAVE0"));
  delay(50);

  // Set 38400 Baud
  com.print(F("AT+BAUD2"));
  delay(50);

  // Reset, thus restart module.
  com.print(F("AT+RESET"));
  delay(50);

  // Clear read buffer
  while (com.available())
    com.read();

  // TODO: check AT command answers!
  isConfigured = true;
  return isConfigured;
}


}