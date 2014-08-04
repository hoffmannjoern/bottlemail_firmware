#ifndef WRITE_H
#define WRITE_H

#include "structs.h"


void deleteLastIndex() {
  SdFile file, dir;
  char buf[8] = "";
  ltoa(ini.index--, buf, 16);

  // Open last directory
  dir.open(&root, buf, O_RDONLY);
  // Delete Header, text and pic files
  file.open(&dir, "header", O_RDWR);
  file.remove();
  file.close();
  file.open(&dir, "text", O_RDWR);
  file.remove();
  file.close();
  file.open(&dir, "pic", O_RDWR);
  file.remove();
  file.close();
  
  // Delete directory
  dir.rmDir();
  
  // update index in config.ini
  file.open(&root, "config.ini", O_RDWR);
  file.write((void *) &ini.index, sizeof(ini));
  file.close();
}

/**
 * \addtogroup	write_messages Write messages
 * 
 * This group uses the ID_t 0x02xx.
 * 
 * @{
 */
     
     
/** \fn writeMessageHeader()
 *  Call this function with ID_t = 0x0201 and valid message header. See structs for pattern of message header.
 */
void writeMessageHeader() {
  SdFile file, dir;
  /* Pointer to the beginning of message (needed for crcappen) */
  char *ptr = &tx_pWrite[0];
  
  // 1 (status) + 8 (time) + 1 (authorlength) + 1 (textlength) + 8 (textlength) + 8 (picLength) + author + title
  uint32_t freeSpace = (volume.freeClusterCount() * volume.blocksPerCluster()) >> 1;
 // Serial.print("authorlength:");
  //Serial.println(incPtr(rx_Buf, msg.data, 9)[0]);
  //Serial.print("titlelength:");
  //Serial.println(incPtr(rx_Buf, msg.data, 10)[0]);

  /* Calculate size of header */
  uint16_t size = 1 + 8 + 1 + 1 + 8 + 8 + incPtr(rx_Buf, msg.data, 9)[0] + incPtr(rx_Buf, msg.data, 10)[0];

  /* Shift data into text and picture size */
  textLeft = uint8_t (incPtr(rx_Buf, msg.data, 11)[0]) >> 56 | uint8_t (incPtr(rx_Buf, msg.data, 12)[0]) >> 48 | uint8_t (incPtr(rx_Buf, msg.data, 13)[0]) >> 40 | uint8_t (incPtr(rx_Buf, msg.data, 14)[0]) >> 32
              | uint8_t (incPtr(rx_Buf, msg.data, 15)[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data, 16)[0]) >> 16 | uint8_t (incPtr(rx_Buf, msg.data, 17)[0]) >> 8 | uint8_t (incPtr(rx_Buf, msg.data, 18)[0]);
  pictureLeft = uint8_t (incPtr(rx_Buf, msg.data, 19)[0]) >> 56 | uint8_t (incPtr(rx_Buf, msg.data, 20)[0]) >> 48 | uint8_t (incPtr(rx_Buf, msg.data, 21)[0]) >> 40 | uint8_t (incPtr(rx_Buf, msg.data, 22)[0]) >> 32
              | uint8_t (incPtr(rx_Buf, msg.data, 23)[0]) >> 24 | uint8_t (incPtr(rx_Buf, msg.data, 24)[0]) >> 16 | uint8_t (incPtr(rx_Buf, msg.data, 25)[0]) >> 8 | uint8_t (incPtr(rx_Buf, msg.data, 26)[0]);

  /* Check if enough space is available */
  if (freeSpace < textLeft) {
    writeErrorTX(E_NO_SPACE_ON_SD);
    textLeft = 0;
    pictureLeft = 0;
  } else {
    if (textLeft == 0 && pictureLeft == 0){
      writeErrorTX(E_EMPTY_DATA);
    } else {
      /* Create new directory */
      char buf[8] = "";
      ltoa(++ini.index, buf, 16);
      
      /* Create new directory in root, open it and create new header file */
      dir.makeDir(&root, buf);
      dir.open(&root, buf, O_RDONLY);
      file.open(&dir, "header", O_CREAT | O_RDWR);
      
      /* Write data to file */
      for (uint32_t i = 0; i < size; i++) {
        file.write(incPtr(rx_Buf, msg.data, i)[0]);
      }
      file.close();
      
      file.open(&root, "config.ini", O_RDWR);
      file.write((void *) &ini.index, sizeof(ini));
      file.close();
      
      writeTXBuf("$\x02\x01\x00\x00!", 6);
      /* CRC will be directly appended to tx buffer (6 is the length of message without crc) */
      crcappend(tx_Buf, ptr, 6);
      /* Because of crcappend, pWrite pointer needs be incremented two positions */
      tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
    } // End ELSE-Case text and picture size not equal to zero
  } // End ELSE-Case enough space
}

/** \fn writeMessageTextBlock()
 *  Call this function with ID_t = 0x0202 and a block of text data.
 *  !Before you can call this function you have to writeMessageHeader()!
 */
void writeMessageTextBlock() {
  SdFile dir, file;
  /* Pointer to the beginning of message (needed for crcappen) */
  char *ptr = &tx_pWrite[0];
  /* Character array to hold name of last index directory */
  char buf[8] = "";
  
  /* Decide if current block should be last one */
  uint16_t t = min(textLeft, MSG_SIZE);
  
  /* Get last index as HEX name */
  ltoa(ini.index, buf, 16);
  /* Open last directory and text file */
  dir.open(&root, buf, O_RDONLY);
  file.open(&dir, "text", O_CREAT | O_APPEND | O_RDWR);
  
  for (uint16_t i = 0; i < t; i++) {
    file.write(incPtr(rx_Buf, msg.data, i)[0]);
  }
  file.close();
  
  /* New size of left text size */
  textLeft -= t;
  if (DEBUG_MODE) {
    Serial.print("textLeft:");
    Serial.println(uint32_t(textLeft>>32));
    Serial.println(uint32_t(textLeft));
  }
  writeTXBuf("$\x02\x02\x00\x00!", 6);
  /* CRC will be directly appended to tx buffer (6 is the length of message without crc) */
  crcappend(tx_Buf, ptr, 6);
  /* Because of crcappend, pWrite pointer needs be incremented two positions */
  tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
}

/** \fn writeMessagePictureBlock()
 *  Call this function with ID_t = 0x0203 and a block of picture data.
 *  !Before you can call this function you have to writeMessageHeader()!
 */
void writeMessagePictureBlock() {
  SdFile dir, file;
  /* Pointer to the beginning of message (needed for crcappen) */
  char *ptr = &tx_pWrite[0];
  /* Character array to hold name of last index directory */
  char buf[8] = "";
  
  /* Decide if current block should be last one */
  uint16_t t = min(pictureLeft, MSG_SIZE);
  
  /* Get last index as HEX name */
  ltoa(ini.index, buf, 16);
  /* Open last directory and text file */
  dir.open(&root, buf, O_RDONLY);
  file.open(&dir, "pic", O_CREAT | O_APPEND | O_RDWR);
  
  for (uint16_t i = 0; i < t; i++) {
    file.write(incPtr(rx_Buf, msg.data, i)[0]);
  }
  file.close();
  
  /* New size of left text size */
  pictureLeft -= t;
  if (DEBUG_MODE) {
    Serial.print("pictureLeft:");
    Serial.println(uint32_t(pictureLeft>>32));
    Serial.println(uint32_t(pictureLeft));
  }
  writeTXBuf("$\x02\x03\x00\x00!", 6);
  /* CRC will be directly appended to tx buffer (6 is the length of message without crc) */
  crcappend(tx_Buf, ptr, 6);
  /* Because of crcappend, pWrite pointer needs be incremented two positions */
  tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
}

/** \fn write()
 *  Call this set of functions with ID_t = 0x02XX to write a message to the bottle.
 */
void write (){
  switch(incPtr(rx_Buf, msg.ID, 1)[0]) {
    case(0x01)  : writeMessageHeader();
                  break;
    case(0x02)  : writeMessageTextBlock();
                  break;
    case(0x03)  : writeMessagePictureBlock();
                  break;
    default     : writeErrorTX(E_UNKOWN_COMMAND);
                  break;
  }
}

/** @} */ // end of group write_messages
#endif /* WRITE.H */
