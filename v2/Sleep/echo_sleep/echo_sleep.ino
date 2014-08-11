// Sleeping
#include <avr/sleep.h>
#include <avr/interrupt.h>

//Software Serial Port
#include <SoftwareSerial.h>   
#define RxD 0
#define TxD 1
 
#define DEBUG_ENABLED  1
int wakePin = 2;
 
SoftwareSerial BLE(RxD,TxD);
 
void setupBleConnection()
{
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);

  BLE.begin(9600); //Set BLE BaudRate to default baud rate 9600
  /*
  BLE.print("AT+CLEAR"); //clear all previous setting
  BLE.print("AT+ROLE0"); //set the bluetooth name as a slaver
  BLE.print("AT+NAMEBottlemail");
  BLE.print("AT+SAVE0"); // save the connect information
  */
}

void setup() 
{ 
  pinMode(wakePin, INPUT);
  setupBleConnection();
}

void loop() 
{
  sleep_now();
  BLE.print("Hello from Bottl(e)mail, sleeping again.");
} 

void wakeup_isr()
{
  sleep_disable();
  detachInterrupt(0);
}

void sleep_now()
{
  /* Set sleep mode
   * The 5 different modes are:
   *     SLEEP_MODE_IDLE         -the least power savings 
   *     SLEEP_MODE_ADC
   *     SLEEP_MODE_PWR_SAVE
   *     SLEEP_MODE_STANDBY
   *     SLEEP_MODE_PWR_DOWN     -the most power savings
   */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  
  // Sleep is now possible
  sleep_enable();
  
  // Set ISR
  /* Enable a interrupt.
   * attachInterrupt(A, B, C)
   * A   can be either 0 or 1 for interrupts on pin 2 or 3.   
   * 
   * B   Name of a function you want to execute while in interrupt A.
   *
   * C   Trigger mode of the interrupt pin. can be:
   *             LOW        a low level trigger
   *             CHANGE     a change in level trigger
   *             RISING     a rising edge of a level trigger
   *             FALLING    a falling edge of a level trigger
   *
   * In all but the IDLE sleep modes only LOW can be used.
   */
  attachInterrupt(wakePin % 2, wakeup_isr, CHANGE);

  // Put device to sleep
  sleep_cpu();
}
