#ifndef READ_H
#define READ_H

#include "structs.h"

bool readINI() {
  SdFile file;
  // Try to open *.ini file
  if (!file.open(&root, "config.ini", O_READ)) {
    if (DEBUG_MODE) {
      Serial.println("Error: cannot open *.ini");
    }
    return false;
  } 
  
  /* Char array to read 4 Byte of index information */      
  char buf[4];
  file.read((void *) buf, 4);
        
  /* Transform and save 4 byte information about last index */
  ini.index = ((uint32_t (buf[3]))<<24) | ((uint32_t (buf[2]))<<16) | ((uint32_t (buf[1]))<<8) | (uint32_t (buf[0]));
        
  file.close();
  return true;
}

/**
 * \addtogroup	read_messages Read messages
 * 
 * This group uses the ID_t 0x01xx.
 *
 * @{
 */

/** \fn readNumberOfMessages()
 *  Call this function with ID_t = 0x0101
 *  Get the number of messages on the bottle with 4 bytes
 */
void readNumberOfMessages() {
  /* Pointer to the beginning of message (needed for crcappen) */
  char *ptr = &tx_pWrite[0];
  char buf[4];
  if (DEBUG_MODE) { 
    Serial.print("number of messages = "); 
    Serial.println(ini.index); 
  }
  buf[0] = ini.index >> 24;
  buf[1] = ini.index >> 16;
  buf[2] = ini.index >> 8;
  buf[3] = ini.index;
  
  /* Write start character, ID for readNumberOfMessages method (0x0101) and the length of the following data (4 Bytes) */
  writeTXBuf("$\x01\x01\x00\x04", 5);
  writeTXBuf (buf, 4);
  writeTXBuf("!", 1);
  
  /* CRC will be directly appended to tx buffer (10 is the length of message without crc)
   * But tx_pWrite therefore must be incremented 2 positions */
  crcappend(tx_Buf, ptr, 10);
  tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
}

/** \fn readMessageHeader()
 *  Call this function with ID_t = 0x0102 and desired message ID with 4 byte value.
 *  Get the header information of selected message back. See structs for pattern of message header.
 */
void readMessageHeader() {
  /* msg.data is supposed to contain message ID */
  SdFile dir, file;
  /* Save message ID as character array */
  uint32_t index;
  /* Buffer to store index characters as HEX */
  char buf[8] = "";
  index = uint8_t (msg.data[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data)[0]) >> 16 | uint8_t(incPtr(rx_Buf, msg.data, 2)[0]) >> 8 | incPtr(rx_Buf, msg.data, 3)[0];

  ltoa(index, buf, 16);
  
  /* Try to open message directory */
  if (!dir.open(&root, buf, O_RDONLY)) {
    if (DEBUG_MODE) {
      Serial.println("Error: Message ID not found.");
    }
    writeErrorTX(E_ID_ERROR);
  } else {
    /* Try to open header file */
    if (!file.open(&dir, "header", O_RDONLY)) {
      if (DEBUG_MODE) {
        Serial.println("Error: Header not found.");
      }
      writeErrorTX(E_ID_ERROR);
    } else {
        /* Pointer to the beginning of message (needed for crcappen) */
        char *ptr = &tx_pWrite[0];
        
        char size[2];
        size[0] = (file.fileSize() << 8);
        size[1] = file.fileSize();
        
        /* Write start character, ID for readNumberOfMessages method (0x0102) */
        writeTXBuf("$\x01\x02", 3);
        
        if (DEBUG_MODE) {
          Serial.print("Filesize: ");
          Serial.print(file.fileSize());
        }
        
        /* Write length of following header data (2 bytes) */
        writeTXBuf((char *) size, 2);
        
        /* Read header information and write them to tx buffer */
        for (int i=0; i < file.fileSize(); i++){
          char c[1];
          c[0] = file.read();
          writeTXBuf (c, 1);
        }
        
        /* Write end character */
        writeTXBuf("!", 1);
  
        /* CRC will be directly appended to tx buffer (length is 5 bytes + fileSize + 1)
         * But tx_pWrite therefore must be incremented 2 positions */
        crcappend(tx_Buf, ptr, 5 + file.fileSize() + 1);
        tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
    } // End reading header
    file.close(); 
  } // End opening header file
  dir.close();
}

/** \fn readMessageTextBlock()
 *  Call this function with ID_t = 0x0103, desired message ID with 4 byte value and desired block of the text with 4 byte value (starts with zero).
 *  Get the selected text block back.
 */
void readMessageTextBlock() {
  /* msg.data is supposed to contain message ID and blockcount */
  SdFile dir, file;
  /* Save message ID */
  uint32_t index;
  /* Save block count  */
  uint32_t block;
  /* Buffer to store index characters as HEX */
  char buf[8] = "";
  
  index = uint8_t (msg.data[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data)[0]) >> 16 | uint8_t(incPtr(rx_Buf, msg.data, 2)[0]) >> 8 | incPtr(rx_Buf, msg.data, 3)[0];
  block = uint8_t (incPtr(rx_Buf, msg.data, 4)[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data, 5)[0]) >> 16 | uint8_t(incPtr(rx_Buf, msg.data, 6)[0]) >> 8 | incPtr(rx_Buf, msg.data, 7)[0];
  
  ltoa(index, buf, 16);
  /* Try to open message directory */
  if (!dir.open(&root, buf, O_RDONLY)) {
    if (DEBUG_MODE) {
      Serial.println("Error: Message ID not found.");
    }
    writeErrorTX(E_ID_ERROR);
  } else {
    /* Try to open text file */
    if (!file.open(&dir, "text", O_RDONLY)) {
      if (DEBUG_MODE) {
        Serial.println("Error: Text not found.");
      }
      writeErrorTX(E_ID_ERROR);
    } else {
        /* Set files position according to given block */
        if (block != 0 && file.seekSet(MSG_SIZE * (block)) == false) {
          writeErrorTX(E_BLOCKCOUNT);
        } else {
          /* Pointer to the beginning of message (needed for crcappen) */
          char *ptr = &tx_pWrite[0];
          
          uint16_t s = min(MSG_SIZE, file.fileSize() - MSG_SIZE * block);

          char size[2];
          /* Write start character, ID for readTextBlock method (0x0103) */
          writeTXBuf("$\x01\x03", 3);
  
          size[0] = (s << 8);
          size[1] = s;          

          writeTXBuf(size, 2);
          
          /* Read data and write them to tx buffer */
          for (int i=0; i < s; i++){
            char c[1];
            c[0] = file.read();
            writeTXBuf (c, 1);
          }
          /* Write end character */
          writeTXBuf("!", 1);
    
          /* CRC will be directly appended to tx buffer (length is 5 bytes + fileSize + 1)
           * But tx_pWrite therefore must be incremented 2 positions */
          crcappend(tx_Buf, ptr, 5 + s + 1);
          tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
        }
    } // End reading file
    file.close(); 
  } // End opening file
  dir.close();
}

/** \fn readMessagePictureBlock()
 * Call this function with ID_t = 0x0104, desired message ID with 4 byte value and desired block of the picture with 4 byte value (starts with zero).
 * Get the selected picture block back.
 */
void readMessagePictureBlock() {
  /* msg.data is supposed to contain message ID and blockcount */
  SdFile dir, file;
  /* Save message ID */
  uint32_t index;
  /* Save block count  */
  uint32_t block;
  /* Buffer to store index characters as HEX */
  char buf[8] = "";
  
  index = uint8_t (msg.data[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data)[0]) >> 16 | uint8_t(incPtr(rx_Buf, msg.data, 2)[0]) >> 8 | incPtr(rx_Buf, msg.data, 3)[0];
  block = uint8_t (incPtr(rx_Buf, msg.data, 4)[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data, 5)[0]) >> 16 | uint8_t(incPtr(rx_Buf, msg.data, 6)[0]) >> 8 | incPtr(rx_Buf, msg.data, 7)[0];
  
  ltoa(index, buf, 16);

  /* Try to open message directory */
  if (!dir.open(&root, buf, O_RDONLY)) {
    if (DEBUG_MODE) {
      Serial.println("Error: Message ID not found.");
    }
    writeErrorTX(E_ID_ERROR);
  } else {
    /* Try to open text file */
    if (!file.open(&dir, "pic", O_RDONLY)) {
      if (DEBUG_MODE) {
        Serial.println("Error: Picture not found.");
      }
      writeErrorTX(E_ID_ERROR);
    } else {
        /* Set files position according to given block */
        if (block != 0 && file.seekSet(MSG_SIZE * (block)) == false) {
          writeErrorTX(E_BLOCKCOUNT);
        } else {
          /* Pointer to the beginning of message (needed for crcappen) */
          char *ptr = &tx_pWrite[0];
          
          uint16_t s = min(MSG_SIZE, file.fileSize() - MSG_SIZE * block);

          char size[2];
          /* Write start character, ID for readMessagePictureBlock method (0x0104) */
          writeTXBuf("$\x01\x04", 3);
  
          size[0] = (s << 8);
          size[1] = s;          

          writeTXBuf(size, 2);
          
          /* Read data and write them to tx buffer */
          for (int i=0; i < s; i++){
            char c[1];
            c[0] = file.read();
            writeTXBuf (c, 1);
          }
          /* Write end character */
          writeTXBuf("!", 1);
    
          /* CRC will be directly appended to tx buffer (length is 5 bytes + fileSize + 1)
           * But tx_pWrite therefore must be incremented 2 positions */
          crcappend(tx_Buf, ptr, 5 + s + 1);
          tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
        }
    } // End reading file
    file.close(); 
  } // End opening file
  dir.close();
}

/** \fn read()
 * Call this set of functions with ID_t = 0x01XX for reading messages from the bottle.
*/
void read (){
  switch(incPtr(rx_Buf, msg.ID, 1)[0]) {
    case(0x01)  : readNumberOfMessages();
                  break;
    case(0x02)  : readMessageHeader();
                  break;
    case(0x03)  : readMessageTextBlock();
                  break;
    case(0x04)  : readMessagePictureBlock();
                  break;
    default     : writeErrorTX(E_UNKOWN_COMMAND);
                  break;
  }
}


/** @} */ // end of group read_messages
#endif /* READ.H */
