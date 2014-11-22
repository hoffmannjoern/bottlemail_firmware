#ifndef _SDCARD_H__
#define _SDCARD_H__

#include <SdFat.h>

namespace BottleMail {
  
class SDCard
{
  public:
  SDCard(uint8_t chipSelectPin) : chipSelectPin(chipSelectPin) {};
  
  inline bool initialize()
  {
    return sd.begin(chipSelectPin, SPI_HALF_SPEED);
  }
  
inline const void listRootDir()
  {
    Serial.println(F("Name          Modify Date/Time    Size"));
    sd.ls("/", LS_R | LS_DATE | LS_SIZE);
  }
  
  private:
  SdFat sd;
  const uint8_t chipSelectPin;
};

}
#endif
