#ifndef STRUCTS.H
#define STRUCTS.H

#include <SdFat.h>

//#include <EEPROM.h>
//#include <Arduino.h>
#define SERIAL_DELAY 5
#define SD_PIN 4
#define DEBUG_MODE true
#define READ_EEPROM false
#define WRITE_EEPROM false
#define MSG_SIZE 300

/**
     * \addtogroup	errors Error codes
     *
     * This group uses the ID FFFF.
     * 
     * @{
     */
     /* #define E_GENERAL_SD_CARD_ERROR 0x0001 // Triggered, if something is wrong with the SD card, e.g. SD card not available
      #define E_GENERAL_LOW_BATTERY 0x0002 // Triggered, if the battery have to be changed
      #define E_GENERAL_UNKOWN_COMMAND 0x0003 // Triggered, if the received command is unknown
      #define E_GENERAL_CRC_ERROR 0x0004 // Triggered, if message has the wrong crc sum
      #define E_READ_UNKOWN_COMMAND 0x0003 // Triggered, if the received read command is unknown
      #define E_READ_CRC_ERROR 0x0004 // Triggered, if message has the wrong crc sum
      #define E_WRITE_HEADER_NO_SPACE_ON_SD 0x0001 // Triggered, if there is not enough space left on SD card for the message
      #define E_WRITE_HEADER_INVALID_DATE 0x0002 // Triggered, if the received date information is invalid
      #define E_WRITE_HEADER_INVALID_TEXT_DATA 0x0003
      #define E_WRITE_TEXT_DATA_SIZE 0x0001
      #define E_WRITE_TEXT_BLOCK_COUNT 0x0002
      #define E_WRITE_PIC_DATA_SIZE 0x0001
      #define E_WRITE_PIC_BLOCK_COUNT 0x0002
      #define E_WRITE_UNKOWN_COMMAND 0x0005
      #define E_WRITE_CRC_ERROR 0x0006 // Triggered, if message has the wrong crc sum
      #define E_WRITE_LAST_MESSAGE_INCOMPLETE 0x0007
      #define E_WRITE_NO_HEADER_WRITTEN 0x0008 */
      
      /** \def E_ID
       * General error ID = 0xFFFF
       */
      #define E_ID 0xFFFF
      
      /** \def E_SD_CARD_ERROR
       * Triggered, if something is wrong with the SD card, e.g. SD card not available
       * E_SD_CARD_ERROR = 0x0001
       */
      #define E_SD_CARD_ERROR 0x0001
      
      /** \def E_LOW_BATTERY
       * Triggered, if the battery have to be changed (not implemented yet)
       * E_LOW_BATTERY = 0x0002
       */
      #define E_LOW_BATTERY 0x0002
      
      /** \def E_UNKOWN_COMMAND
       * Triggered, if the received command is unknown
       * E_UNKOWN_COMMAND = 0x0003
       */
      #define E_UNKOWN_COMMAND 0x0003
      
      /** \def E_CRC_ERROR
       * Triggered, if message has the wrong crc sum
       * E_CRC_ERROR = 0x0004
       */
      #define E_CRC_ERROR 0x0004
      
      /** \def E_NO_SPACE_ON_SD
       * Triggered, if there is not enough space left on SD card for the message
       * E_NO_SPACE_ON_SD = 0x0005
       */
      #define E_NO_SPACE_ON_SD 0x0005
      
      /** \def E_INVALID_DATE
       * Triggered, if the received date information is invalid (not implemented yet)
       * E_INVALID_DATE = 0x0006
       */
      #define E_INVALID_DATE 0x0006
      
      /** \def E_TEXT_DATA_SIZE
       * Triggered, if text length not equal to received data
       * E_TEXT_DATA_SIZE = 0x0007
       */
      #define E_TEXT_DATA_SIZE 0x0007
      
      /** \def E_PIC_DATA_SIZE
       * Triggered, if picture length not equal to received data
       * E_PIC_DATA_SIZE = 0x0008
       */
      #define E_PIC_DATA_SIZE 0x0008
      
      /** \def E_LAST_MESSAGE_INCOMPLETE
       * Triggered, if last message was not completed
       * E_LAST_MESSAGE_INCOMPLETE = 0x0009
       */
      #define E_LAST_MESSAGE_INCOMPLETE 0x0009
      
      /** \def E_EMPTY_DATA
       * Triggered, if length of text and picture are zero
       * E_EMPTY_DATA = 0x000a
       */
      #define E_EMPTY_DATA 0x000a 
      
      /** \def E_ID_ERROR
       * Triggered, if message id was not found or message is corrupt
       * E_ID_ERROR = 0x000b
       */
      #define E_ID_ERROR 0x000b
      
      /** \def E_UNEXPECTED_MSG
       * Triggered, if a specific answer is expected, but got something different
       * E_UNEXPECTED_MSG = 0x000c
       */
      #define E_UNEXPECTED_MSG 0x000c// 
      /** @} */ // end of group write_messages

typedef uint16_t ID_t;
  
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
// Serial streams
//ArduinoOutStream cout(Serial);


  
    /**
     * \addtogroup	general General information
     * 
     * This group uses the ID 00xx.
     * 
     * @{
     */

	struct protocolVersion {
	  uint8_t	minor;
	  uint8_t	major;
	};

	struct bottleID {
	  uint32_t	bottleID;
	};

    /** @} */ // end of group general



    /**
     * \addtogroup	read_messages Read messages
     * 
     * This group uses the ID 01xx.
     * 
     * @{
     */

	struct numberOfMessages {
	  uint32_t	number;
	};

	/*struct messageBlockEntry {
	  uint32_t		messageID;
	  uint8_t		titleLength;	//amount of bytes
	  char*	                title;		//as many bytes specified by titleLength without trailing \0
	  int64_t		date;		//64 bit unix timestamp
	};*/

	struct messageHeader {
          uint64_t              date;
	  uint8_t		authorLength;	//amount of bytes without trailing \0
	  uint8_t		titleLength;	//amount of bytes without trailing \0
	  uint64_t		textLength;	//amount of bytes without trailing \0, has value 0 if no text available
	  uint64_t		pictureLength;	//picture size in bytes, has value 0 if no picture available
          uint32_t		messageID;
	  char*	                author;
	  char*	                title;
	};

	/*struct messageText {
	  char*	        text;
	};

	struct messagePicture {
	  char*	        picture;
	};*/

        union messageData {
	  char        text[MSG_SIZE];
          char        pic[MSG_SIZE];
          uint32_t    blockcount;
	};

    /** @} */ // end of group read_messages



    /**
     * \addtogroup	write_messages Write messages
     * 
     * This group uses the ID 02xx.
     * 
     * @{
     */

	/*struct messageHeaderWritten {
	  uint16_t 		errorcode;
	  uint32_t		messageID;
	};*/

    /** @} */ // end of group write_messages


    struct SDini {
      uint32_t index;
     } ini;
    uint32_t freeSpace;
    uint64_t textLeft;
    uint64_t pictureLeft;

    Sd2Card card;
    SdVolume volume;
    SdFile root;
    bool rdy;

/**
 * \union param
 * This union can contain all possible features of a message.
 * You just need one element of this union at one communication action.
 */
    /*union param {
      protocolVersion pV;
      bottleID bID;
      ID_t errorcode;
      numberOfMessages numbOfMessages;
      messageHeader mHeader;
      messageText text;
      messagePicture pic;
    };*/
    
    union param {
      protocolVersion pV;
      bottleID bID;
      ID_t errorcode;
      numberOfMessages numbOfMessages;
      messageHeader mHeader;
      messageData data;
    };


/**
 * \struct validMessage
 * A validMessage have '$' as start frame and '!' as end frame
 * After start frame follows the ID of the command.
 * After that you can send something that is part of param struct.
 * Now you send the end frame and append the crc sum.
 */
    struct validMessage {
      ID_t ID;
      param p;
      uint16_t crc16;
    } m;

    char* ptr = (char *) &m.p;

  void writeByte(uint8_t value) {
    Serial.write(value);
  }
  void writeByte(uint16_t value) {
    Serial.write(value>>8);
    Serial.write(value);
  }
  void writeByte(uint32_t value) {
    Serial.write(value>>24);
    Serial.write(value>>16);
    Serial.write(value>>8);
    Serial.write(value); 
  }
  void writeByte(uint64_t value) {
    // Serial.write can't work with 64 bit values (maybe because serial buffer only holds 63 bits)
    uint32_t a,b;
    a = value>>32;
    b = value;
    writeByte(a);
    writeByte(b);
  }



  uint64_t swap_uint64(uint64_t val)
  {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
  }

  void writeMessage(validMessage &m, uint16_t length) {
    // First empty serial buffer to ensure no more data is incomming
    do {
      Serial.read();
      if (Serial.available() == 0) delay(SERIAL_DELAY);
    } while (Serial.available() > 0);
              
    Serial.write('$');
    writeByte(m.ID);
 
    for (uint16_t i = length - 1; i < length; i--)
    //for (uint16_t i = 0; i < length; i++)
      Serial.write(ptr[i]);
  
    Serial.write('!');
    writeByte(m.crc16);
  }
  
  
  void writeMessage(validMessage &m, uint16_t length, char *p1, uint16_t l1, char *p2, uint16_t l2) {
    do {
      Serial.read();
      if (Serial.available() == 0) delay(SERIAL_DELAY);
    } while (Serial.available() > 0);
              
    Serial.write('$');
    writeByte(m.ID);
    //writeByte(m.p.mHeader.messageID);
 
    //for (uint16_t i = length - 1; i < length; i--)
    for (uint16_t i = 0; i < length - 4; i++)
      Serial.write(ptr[i]);
    for (uint16_t i = 0; i < l1; i++) {
      Serial.write(p1[i]);
    }
    if (p2 != NULL && l2 != 0) {
      for (uint16_t i = 0; i < l2; i++)
        Serial.write(p2[i]);
    }
  
    Serial.write('!');
    writeByte(m.crc16);
  }

#endif /* STRUCTS.H */

