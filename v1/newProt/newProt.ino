#define C_START '$'
#define C_END '!'
#define BAUD 9600
#define SERIAL_TIMEOUT 100
//#define DEBUG_MODE false


#include <EEPROM.h>
//you have to use the current version of sdfat library. at least sdfat20130629.
#include <SdFat.h>
#include "structs.h"
#include "crc.h"
#include "general.h"
#include "read.h"
#include "write.h"

/* Boolean value to indicate, if SD-Card is ready (true if accessible) */
bool rdy;

void setup() {  
  SdFile file;
  
  pinMode(SD_PIN, OUTPUT);
  /* Initialize serial and wait for port to open */
  Serial.begin(BAUD); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.setTimeout(SERIAL_TIMEOUT);
  
  /* Test if SD card is working */
  if(!card.init(SPI_HALF_SPEED, SD_PIN)){
    if (DEBUG_MODE) {
      Serial.println("Error: SD initialization failed");
    }
    rdy = false;
  } else {
    rdy = true;
  }
  
  /* Try to open the 'volume'/'partition' - it should be FAT16 or FAT32 */
  if (!rdy || !volume.init(&card)) {
    if (DEBUG_MODE) {
      Serial.println("Error: Can't find a valid FAT16/FAT32 partition");
    }
    rdy = false;
  } else {
    rdy = true;
  }
  /* Calculate free Space */ 
  uint32_t freeSpace = (volume.freeClusterCount() * volume.blocksPerCluster()) >> 1;
  root.openRoot(&volume);
  
  /* Check if *.ini file exists */
  if (root.exists("config.ini")) {
    /* Try to read config.ini - in case of error, just try to remove *.ini, reset device and new *.ini will be created */
    if (!readINI()) {
      if (DEBUG_MODE) {
        Serial.println("Error: *.ini file corrupt. Try Reset.");
      }
      rdy = false;
    }
  } else {
    /* Create new *.ini file */
    if (DEBUG_MODE) {
      Serial.println("*.ini does not exist");
    }
    if (!file.open(&root, "config.ini", O_CREAT | O_APPEND | O_WRITE)) {
      if (DEBUG_MODE) {
        Serial.println("Error: *.ini cannot be created");
      }
      rdy = false;
    }
    else {
      /* Write initial ini structure to file */
      char *buf [sizeof(ini)] = {'\0'};
      if (file.write((void *) buf, sizeof(ini)) < sizeof(ini)){
        if (DEBUG_MODE) {
          Serial.println("Error: writing *.ini failed");
        }
      } else if (DEBUG_MODE) {
        Serial.println("writing *.ini successful");
      }
      file.close();
    }
  }
  
  if (DEBUG_MODE) {
    Serial.print("free space: ");
    Serial.print((volume.freeClusterCount() * volume.blocksPerCluster()) >> 1);
    Serial.println(" KB");
  }
  
  /* Write information to EEPROM */
  if (WRITE_EEPROM) {
    writeEEPROM();
    writeCRCTable();
  }
  if (READ_EEPROM) {
    readEEPROM();
  }
}

void loop() {
  if (DEBUG_MODE) {
    Serial.print("New loop:");
    Serial.println(rx_Buf);
  }
    /* Check if there is a start character in transmitted message and at least 8 characters are read into rx buffer (start(1) + ID(2) + length(2) + end(1) + crc(2) = 8) */
  if (*rx_pHead == C_START && abs(&rx_pHead[0] - &rx_pWrite[0]) >= 8){
    /* Save pointers to rx buffer where the appropriate information are supposed to be */
    msg.start = &rx_pHead[0];
    msg.ID = incPtr(rx_Buf, rx_pHead);
    msg.length = incPtr(rx_Buf, rx_pHead, 3);
    msg.data = incPtr(rx_Buf, rx_pHead, 5);
    /* Save function pointer to crcverify */
    msg.crcverify = &crcverify;
    msg.crcappend = &crcappend;
      
    /* Get length of message, to check if enough characters were written in buffer (write pointer needs to be far enough) */
    uint16_t offset = (uint16_t (msg.length[0])<<8 | uint16_t(incPtr(rx_Buf, msg.length)[0])) + 3; // +3 = [C_END] + 2 Byte CRC
    uint16_t diff = (&rx_pHead[0] < &rx_pWrite[0]) ? &rx_pWrite[0] - &rx_pHead[0] : BUF_SIZE - (&rx_pHead[0] - &rx_pWrite[0]); // Number of characters between head pointer and write pointer (is all data available?)
    if (DEBUG_MODE) {
      Serial.print("Offset: ");
      Serial.println(offset);
      Serial.print("diff: ");
      Serial.println(diff);
    }
    msg.end = incPtr(rx_Buf, rx_pHead, 5 + offset-3);
    msg.crc = incPtr(rx_Buf, msg.end);
    /* When head pointer is behind write pointer, then the offset needs to be smaller than diff
     * When Head ptr is in front of write ptr, then offset needs to be smaller than BUF_SIZE - diff
     * In those two cases there are enough characters available to execute command
     * Check if character msg.data+offset is End-Character and then verify crc-sum
     */
    if (offset <= diff) {
      /* Check if SD Card is available () */
      if (rdy == false && *rx_pHead == C_START){
        writeErrorTX(E_SD_CARD_ERROR);
      } else {
          /* Offset +4 : ID (2) + length(2) */
          if (msg.end[0] == C_END && (*msg.crcverify)(rx_Buf, msg.start, offset+4)){
             /* Check if message is incomplete (in this case, delete last index directory) */
             if ((textLeft != 0 || pictureLeft != 0) && msg.ID[0] != 0x02) {
               writeErrorTX(E_LAST_MESSAGE_INCOMPLETE);
               deleteLastIndex();
               textLeft = 0;
               pictureLeft = 0;
             } else {
               switch(msg.ID[0]) {
                 case(0x00)  : general();
                               break;
                 case(0x01)  : read();
                               break;
                 case(0x02)  : write();
                               break;
                 default     : writeErrorTX(E_UNKOWN_COMMAND);
                               break;
               } // End switch
             }
          } // End IF-CASE end-character, crcverify
          else {
            writeErrorTX(E_CRC_ERROR);
          }
        } // End ELSE-CASE sd check
        /* Set rx head pointer to 0, to indicate, that message was processed */
        *rx_pHead = '\0';
        /* Set read pointer of rx buffer to end of received message */
        rx_pRead = incPtr(rx_Buf, msg.crc, 2);        
      } // End IF-Case offset, diff
      else {
        *rx_pHead = '\0';
        /* NOT enough data in buffer -> do nothing */
      }
    } // End start character
    
delay(SERIAL_TIMEOUT);

  /* Check if there is a message in output buffer */
  for (int i=0; i < BUF_SIZE; i++){
    if (*tx_pRead == C_START) {
      while (&tx_pRead[0] != &tx_pWrite[0]) {
        Serial.write(*tx_pRead);
        /* Delete character from tx_Buf to avoid infinite loops over buffer */
        *tx_pRead = '\0';
        tx_pRead = incPtr(tx_Buf, tx_pRead);
      } // End while loop
    } 
  } // End loop i
  
  /* Check if there is a start character in input buffer */
  for (int i=0; i < BUF_SIZE; i++) {
    if (*rx_pRead == C_START) {
      rx_pHead = &rx_pRead[0];
    } else {
        rx_pRead = incPtr(rx_Buf, rx_pRead);
    }
  }
}

/* serialEvent() will be triggered, iff Serial.available()>0 after loop() method
 * Copy available characters into buffer
 */
void serialEvent() {
  while (Serial.available()){
    /* Check if next memory address is free to write (not the beginning of a message) */
    if (&incPtr(rx_Buf, rx_pWrite)[0] != &rx_pHead[0] || *rx_pHead != C_START) {
      *rx_pWrite = Serial.read();
      rx_pWrite = incPtr(rx_Buf, rx_pWrite);
    } else {
      break;
    }
  }
}

void readEEPROM () {
  if (DEBUG_MODE) {
    Serial.println("Output EEPROM:");
  }
  int address = 0;
  while (address < 512){
    Serial.print(address);
    Serial.print("\t");
    Serial.write(EEPROM.read(address));
    address++;
  }
}

void writeEEPROM () {
  EEPROM.write(0, BOTTLE_ID >> 24);
  EEPROM.write(1, BOTTLE_ID >> 16);
  EEPROM.write(2, BOTTLE_ID >> 8);
  EEPROM.write(3, BOTTLE_ID);
  EEPROM.write(4, PV_Major);
  EEPROM.write(5, PV_Minor);
}
