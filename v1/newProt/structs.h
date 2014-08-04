#ifndef STRUCTS_H
#define STRUCTS_H

#define SD_PIN 4
#define BAUD 9600
#define SERIAL_TIMEOUT 100
#define DEBUG_MODE false
#define READ_EEPROM false
#define WRITE_EEPROM false
#define BOTTLE_ID 0x00000001
#define PV_Major 0x00
#define PV_Minor 0x01
#define MSG_SIZE 400
#define BUF_SIZE (5 + MSG_SIZE + 3 + 1)  // Buffer needs to be at least MSG_SIZE + start character (1) + ID (2) + length (2) + end (1) + crc (2)

/**
     * \addtogroup	errors Error codes
     *
     * This group uses the ID FFFF.
     * 
     * @{
     */
      
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
      
      /** \def E_BLOCKCOUNT
       * Triggered, if blockcount is invalid (too large for file size)
       * E_BLOCKCOUNT = 0x000d
       */
      #define E_BLOCKCOUNT 0x000d
      
      /** @} */ // end of group write_messages

typedef uint16_t ID_t;

/* Circular buffer with one slot open to avoid confusion when two pointer point to same element (would be not clear, if buffer is empty or full) 
 * Now if two pointers point to same element, it means that buffer is empty, and if write pointer points to one element preceding the 
 * element of read pointer, buffer is full
 */
char rx_Buf[BUF_SIZE+1];
char tx_Buf[BUF_SIZE+1];

/* pHead points to a start symbol and so to the begin of a message candidate
 * pRead points to current reading position
 * pWrite points to current writing position
 */
char *rx_pHead = &rx_Buf[0];
char *rx_pRead = &rx_Buf[0];
char *rx_pWrite = &rx_Buf[0];
char *tx_pHead = &tx_Buf[0];
char *tx_pRead = &tx_Buf[0];
char *tx_pWrite = &tx_Buf[0];

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

struct Message {
  /* Length to Integer : (uint16_t (msg->length<<8) | uint16_t(msg->length>>8)) */
  char *start;
  char *ID;
  char *length;
  char *data;
  char *end;
  char *crc;
  bool (*crcverify)(char *buf, char* data, uint16_t length);
  void (*crcappend)(char *buf, char* data, uint16_t length);

} __attribute__ ((packed)) msg;


/**
 * \struct validMessage
 * This is the pattern of a valid message.
 * A validMessage have '$' as start frame and '!' as end frame
 * After start frame follows the ID_t of the command as 2 byte value. Afterwards you define the length of your message as 2 byte value.
 * Now follows the data to transmit.
 * After end frame the crc is added.
 */
struct validMessage {
	char start;
	uint16_t ID_t;
	uint16_t messageLength;
	char *data;
	char end;
	uint16_t crc;
};

/**
 * \struct MessageHeader
 * This is the pattern of a valid message header.
 */
struct MessageHeader {
	char        statusByte;     //this byte shows the status of the message; default / message ok: 0x00; test message: 0x54
	uint64_t    date;	    //unix timestamp
	uint8_t	    authorLength;   //amount of bytes of author's name without trailing \0
	uint8_t	    titleLength;    //amount of bytes of title without trailing \0
	uint64_t    textLength;	    //size of text in bytes without trailing \0 (0 if no text available)
	uint64_t    pictureLength;  //size of picture in bytes, (0 if no picture available)
	char*	    author;	    //author (needs to be as long as defined with authorLength)
	char*	    title;	    //title (needs to be as long as defined with titleLength)
};

/* Structure for config file on sd volume to store index of last saved message */
struct SDini {
  uint32_t index;
} ini;

/* Current number of text bytes left to complete writing of current message */
uint64_t textLeft = 0;
/* Current number of picture bytes left to complete writing of current message */
uint64_t pictureLeft = 0;

/* SD Card objects to access SD Card information */
Sd2Card card;
SdVolume volume;
SdFile root;

/* Method to swap byte order of given uint16_t value */
uint16_t swap_uint16 (uint16_t val) {
  return ( ((val << 16) & 0xFFFF) | ((val >> 16) & 0xFFFF));
}

/* Return true if given pointer is last memory address of buffer */
bool bufEnd(char *buf, char *p) {
  return (&p[0] == &buf[BUF_SIZE - 1]) ? true : false;
}

/* Return incremented given pointer to next position in buffer */
char * incPtr(char *buf, char *p) {
  return (&p[0] == &buf[BUF_SIZE - 1]) ? buf : p+1;
}
/* Return incremented given pointer to next position in buffer */
char * incPtr(char *buf, char *p, uint16_t offset) {
  if (offset == 0) {
    return &p[0];
  }
  return &buf[((p - buf + offset) % BUF_SIZE)];
}

/* Write given message to tx buffer from current position of write pointer 
 *(there is no check if buffer is big enough, so this should be guaranteed by setting BUF-SIZE high enough) 
 */
void writeTXBuf(char *msg, uint16_t length) {
  /* Check if tx write pointer is at the end of circular buffer */
/*  if (bufEnd(tx_Buf, tx_pWrite)) {
    incPtr(tx_Buf, tx_pWrite);
  }*/
  /* Write each character after another into tx buffer */
  for (uint16_t i = 0; i < length; i++) {
    *tx_pWrite = msg[i];
    tx_pWrite = incPtr(tx_Buf, tx_pWrite);
  }
}

/* Write a whole message directly to tx_buf with given errorcode (or other uint16_t value)
 * @param    uint16_t    2 byte errorcode according to #define rules
 */
void writeErrorTX(uint16_t error) {
  char buf[2];
  char *p = &tx_pWrite[0];
  writeTXBuf("$\xFF\xFF\x00\x02", 5);
  buf[0] = error >> 8;
  buf[1] = error;
  writeTXBuf(buf, 2);
  writeTXBuf("!", 1);
  (*msg.crcappend)(tx_Buf, p, 8);
  tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
}

#endif /* STRUCTS.H */

