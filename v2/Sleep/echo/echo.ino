
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 0
#define TxD 1
 
#define DEBUG_ENABLED  1
 
SoftwareSerial BLE(RxD,TxD);

void setupBleConnection()
{
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  BLE.begin(9600);       // Set BLE BaudRate to default baud rate 9600
  
  /*
  BLE.print("AT+CLEAR"); // clear all previous setting
  BLE.print("AT+ROLE0"); // set the bluetooth name as a slaver
  BLE.print("AT+NAMEBottlemail");
  BLE.print("AT+SAVE0"); // save the connect information
  */
}

void setup() 
{ 
  setupBleConnection(); 
} 
 
void loop() 
{ 
  char recvChar;
  while(1)
  {
    //check if there's any data sent from the remote BLE shield
    if(BLE.available())
    {
      recvChar = BLE.read();
      BLE.print(recvChar);
    }
  }
}
