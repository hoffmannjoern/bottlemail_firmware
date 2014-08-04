#define BAUD 9600
#define SERIAL_TIMEOUT 100
#define DEBUG_MODE true

#include <EEPROM.h>
//you have to use the current version of sdfat library. at least sdfat20130629.
#include <SdFat.h>
#include "structs.h"
#include "crc.h"
#include "general.h"
#include "read.h"
#include "write.h"

void setup() {    
  SdFile file;
  
 //Initialize serial and wait for port to open:
  Serial.begin(BAUD); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.setTimeout(SERIAL_TIMEOUT);
  
  pinMode(SD_PIN, OUTPUT);
  
  if(!card.init(SPI_HALF_SPEED, SD_PIN)){
    if (DEBUG_MODE) Serial.println("Error: SD initialization failed");
    rdy = false;
  } else {
    //if (DEBUG_MODE) Serial.println("SD Card successfully initialized");
    rdy = true;
  }
  //Serial.println("\n[memcheck]");
  //Serial.println(freeRam());

  // Try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!rdy || !volume.init(&card)) {
    if (DEBUG_MODE) Serial.println("Error: Can't find a valid FAT16/FAT32 partition");
    rdy = false;
  } else {
    rdy = true;
  }
  freeSpace = (volume.freeClusterCount() * volume.blocksPerCluster()) >> 1;
  root.openRoot(&volume);

  //root.remove(&root, INI);
  //root.ls(LS_R | LS_DATE | LS_SIZE);
  
  // Check if *.ini file exists
  if (root.exists("config.ini")) {
    //if (DEBUG_MODE) Serial.println("*.ini already exists");
    // Try to read it - in case of error, just try to remove *.ini, reset device and new *.ini will be created
    if (!readINI()) {
      if (DEBUG_MODE) Serial.println("Error: *.ini file corrupt. Try Reset.");
      rdy = false;
    }
    //else if (DEBUG_MODE) Serial.println("*.ini successfully read");
    
  } else {
    // Create new *.ini file
    if (DEBUG_MODE) Serial.println("*.ini does not exist");
    if (!file.open(&root, "config.ini", O_CREAT | O_APPEND | O_WRITE)) {
      if (DEBUG_MODE) Serial.println("Error: *.ini cannot be created");
      rdy = false;
    }
    else {
      // Write initial ini structure to file
      char *buf [sizeof(ini)] = {'\0'};
      if (file.write((void *) buf, sizeof(ini)) < sizeof(ini)){
        if (DEBUG_MODE) Serial.println("Error: writing *.ini failed");
      } else if (DEBUG_MODE) Serial.println("writing *.ini successful");
      file.close();
    }
  }
  if (DEBUG_MODE) Serial.print("free space: ");
  if (DEBUG_MODE) Serial.print((volume.freeClusterCount() * volume.blocksPerCluster()) >> 1);
  if (DEBUG_MODE) Serial.println(" KB");
  
  //Output EEPROM data
  if (WRITE_EEPROM) {
    //Serial.println(freeRam());
    writeEEPROM();
    writeCRCTable();
  }
  if (READ_EEPROM) readEEPROM();
} 


void loop() {
  // Check if data is available on serial port
  if (Serial.available() > 0) {
  
    // Check first character of incoming serial data
    if (Serial.read() == 0x24) {
      // Delay is necessary because incomming data at serial port takes longer than one loop() cycle 
      // --> at Baud 9600 there are 4 new incomming bytes in the serial buffer after 5ms
      if (Serial.available() < 2) delay(SERIAL_DELAY);
      // Read next two Bytes and shift them into ID
      m.ID = (Serial.read() << 8) | Serial.read();
      
      // If SD card error occured during setup, send sd error back regardless of incomming message
      if (rdy == false) {
        if (Serial.available() == 0) delay(SERIAL_DELAY);
        // Set SD Card error code, append crc sum and write message
        error(E_SD_CARD_ERROR);
        /*m.ID = E_ID;
        m.p.errorcode = E_SD_CARD_ERROR;
        crcappend(m, sizeof(m.p.errorcode));
        writeMessage(m, sizeof(m.p.errorcode));*/
      }
      else {
        switch(uint8_t (m.ID>>8)) {
          case(0x00) :  if (DEBUG_MODE) Serial.println("general");
                        general(m);
                        break;
          case(0x01) :  if (DEBUG_MODE) Serial.println("read");
                        read(m);
                        break;
          case(0x02) :  if (DEBUG_MODE) Serial.println("write");
                        write(m);
                        break;
          default:      
                        error(E_UNKOWN_COMMAND);
                        /*m.ID = E_ID;
                        m.p.errorcode=E_UNKOWN_COMMAND;
                        crcappend(m, sizeof(m.p.errorcode));
                        writeMessage(m, sizeof(m.p.errorcode));*/
        }
      }
    }
   /*else {
     // first character of serial buffer is not '$'
     if (DEBUG_MODE) Serial.println("first character was false");
     m.ID = E_ID;
     m.p.errorcode=E_UNKOWN_COMMAND;
     crcappend(m, sizeof(m.p.errorcode));
     writeMessage(m, sizeof(m.p.errorcode));
    }*/
  }
}

void readEEPROM () {
  Serial.println("Output EEPROM:");
  int address = 0;
  while (address < 512){
    Serial.print(address);
    Serial.print("\t");
    Serial.write(EEPROM.read(address));
    address++;
  }
}

void writeEEPROM () {
  EEPROM.write(0, 0x00);
  EEPROM.write(1, 0x00);
  EEPROM.write(2, 0x00);
  EEPROM.write(3, 0x01);
  EEPROM.write(4, 0x00);
  EEPROM.write(5, 0x01);
}


