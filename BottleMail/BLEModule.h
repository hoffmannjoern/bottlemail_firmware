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
    BLEModule(Stream &com) : com(com), isConfigured(false) {};

    /**
       * Sends the AT command to check if BLE module is answering.
       * @return Returns true if the module is answering, false otherwise
       */
    bool isResponding() const;

    /**
        *  Send initializing commands to the module.
        */
    bool configure();

    /**
        * Returns the set baud rate f the module
        * \return Currently returns 38400 if the is configured or 9600 when not.
        */
    inline uint32_t getBaud() const
    {
      if (isConfigured)
        return 38400;

      else
        return 9600;
    }

  private:
    Stream &com;
    bool isConfigured;
};


}
#endif /* BLEMODULE_H_ */