
#include <SdFile.h>
#include <SdFat.h>

// Sleeping
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "SDCard.h"
#include "FileManager.h"
#include "CommandInterpreter.h"
#include "BLEModule.h"

using namespace BottleMail;

static SDCard sdcard(SS);
static FileManager fileManager;
static CommandInterpreter interpreter(sdcard, fileManager);

/* The pin used to wakeup the board.
  *
  * \note Don't forget to wire the pin with the USART RX pin (pin 0).
  */
static const int wakePin = 2;

/**
  * The interrupt source. Can be either 0 for pin 2 or 1 for pin 3.
  * \note Don't forget to wire the pin with the USART RX pin (pin 0).
  */
static const int wakePinInterruptSource = wakePin % 2;

void setup(void)
{
  BLEModule ble(Serial);

  // Setup the module
  ble.setup();
  sdcard.initialize();
  fileManager.initialize();

  pinMode(wakePin, INPUT);
  Serial.println(F("**** BOTTL(E)MAIL ARDUINO V1 ****\n2K RAM SYSTEM 1024 ANSI-C BYTES FREE\nREADY."));
  delay(500);
}

void loop(void)
{
  while (Serial.available())
  {
    uint8_t byte = Serial.read();

    // ASCII interpreter
    interpreter.processByte(byte);

    // Wait for incoming byte from USART
    // delay(50);
  }

  // Sleep
  //sleep_now();

  // System comes up if byte is send and pin is drained to low
  // Wait for incoming byte from USART
  //delay(50);
}


void wakeup_isr()
{
  sleep_disable();
  detachInterrupt(wakePin);
}

void sleep_now()
{
  /* Set sleep mode
     *
     * The 5 different modes are:
     *    SLEEP_MODE_IDLE                  - the least power savings, UART stays active
     *    SLEEP_MODE_ADC
     *    SLEEP_MODE_STANDBY           - UART disabled, but will come up in 6 cycles, because we will receive an interrupt on RX pin.
     *    SLEEP_MODE_PWR_SAVE         - Same as above, but main clock is disabled. Needs more cycles to settle / adjust quartz
     *    SLEEP_MODE_PWR_DOWN      - the most power savings
     */
  set_sleep_mode(SLEEP_MODE_STANDBY);

  // Sleep is now possible
  sleep_enable();

  /* Set ISR to enable an interrupt
     *
     * attachInterrupt(PIN, FUNCTION, TRIGGEr)
     * pin              Can be either 0 or 1 for interrupts on pin 2 or 3.
     * function       Name of a function you want to execute while in interrupt A.
     * trigger         Trigger mode of the interrupt pin. can be:
     *      LOW           a low level trigger
     *      CHANGE     a change in level trigger
     *      RISING        a rising edge of a level trigger
     *      FALLING     a falling edge of a level trigger
     *
     * NOTE: In all but the IDLE sleep modes only LOW can be used.
     */
  attachInterrupt(wakePinInterruptSource, wakeup_isr, LOW);

  // Put device to sleep
  sleep_cpu();
}




