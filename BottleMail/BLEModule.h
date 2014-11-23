/*
 * BLEModule.h
 *
 * Created: 18.11.2014 11:44:41
 *  Author: joern
 */

#ifndef BLEMODULE_H_
#define BLEMODULE_H_

#include <Arduino.h>

namespace BottleMail {

/**
  * Class to handle the Bluetooth low energy module.
  *
  * \note Currently only the HM-11 Module is supported.
  */
class BLEModule
{
  public:
    BLEModule(HardwareSerial &com) : com(com), configured(false), baud(9600) {};

    /**
       * Sends the AT command to check if BLE module is answering.
       * @return Returns true if the module is answering, false otherwise
       */
    bool isResponding() const;

    /**
        *  Checks if the BLEmodule is responding, otherwise sends configuration commands to the module.
        * \return True if the module is present and if it is configured, false otherwise.
        */
    bool setup();

    /**
        * Returns the set baud rate f the module
        * \return Currently returns 38400 if the is configured or 9600 when not.
        */
    inline const uint32_t &getBaud() const
    {
      return baud;
    }

    /**
         * Returns the configured state
         * @return True if the module is configured.
         */
    inline bool isConfigured() const
    {
      return configured;
    }

  private:
    /**
         *  Send configuration commands to the module.
         */
    bool configure();

    inline void setBaud(uint32_t newBaud)
    {
      baud = newBaud;
      com.begin(baud);
      delay(10);
    }

    HardwareSerial &com;
    bool configured;
    uint32_t baud;
};


}
#endif /* BLEMODULE_H_ */