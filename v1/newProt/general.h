#ifndef GENERAL_H
#define GENERAL_H

#include "structs.h"

/**
 * \addtogroup	general General information
 * 
 * This group uses the ID 0x00xx.
 * 
 * @{
 */

/** \fn readProtocolVersion()
 *  Call this function with ID_t = 0x0001
 *  Get the protocol version with 2 bytes 
 *  @param	validMessage Pointer to message
 *  @return	protocolVersion in major and minor byte to param union
*/
void readProtocolVersion() {
  /* Buffer to store the 2 characters of protocol version from EEPROM */
  char buf[2];
  /* Pointer to the beginning of message (needed for crcappend) */
  char *ptr = &tx_pWrite[0];
  /* Write start character, ID for procotolVersion method (0x0001) and the length of the following data (2 Bytes) */
  writeTXBuf("$\x00\x01\x00\x02", 5);

  buf[0] = EEPROM.read(4);
  buf[1] = EEPROM.read(5);
  writeTXBuf (buf, 2);

  writeTXBuf("!", 1);
  /* CRC will be directly appended to tx buffer (8 is the length of message without crc) */
  crcappend(tx_Buf, ptr, 8);
  /* Because of crcappend, pWrite pointer needs be incremented two positions */
  tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
}

/** \fn readBottleID()
 *  Call this function with ID_t = 0x0002
 *  Get the bootle ID with 4 bytes
 *  @param	validMessage Pointer to message
 *  @return	bottleID to param union
 */
void readBottleID() {
  /* Buffer to store the 4 characters of bottle id from EEPROM */
  char buf[4];
  /* Pointer to the beginning of message (needed for crcappen) */
  char *ptr = &tx_pWrite[0];
  /* Write start character, ID for bottleID method (0x0002) and the length of the following data (4 Bytes) */
  writeTXBuf("$\x00\x02\x00\x04", 5);

  buf[0] = EEPROM.read(0);
  buf[1] = EEPROM.read(1);
  buf[2] = EEPROM.read(2);
  buf[3] = EEPROM.read(3);
  writeTXBuf (buf, 4);

  writeTXBuf("!", 1);
  /* CRC will be directly appended to tx buffer (10 is the length of message without crc)
   * But tx_pWrite therefore must be incremented 2 positions */
  crcappend(tx_Buf, ptr, 10);
  tx_pWrite = incPtr(tx_Buf, tx_pWrite, 2);
  
  /* TODO: write message
  m.p.bID.bottleID = (EEPROM.read(0)<<24) | (EEPROM.read(1)<<16) | (EEPROM.read(2)<<8) | EEPROM.read(3);
  */
}

/** \fn general()
 * Call this set of functions with ID_t = 0x00XX to access general information of the bottle
 */
void general (){
  switch(incPtr(rx_Buf, msg.ID, 1)[0]) {
    case(0x01)  : readProtocolVersion();
                  break;
    case(0x02)  : readBottleID();
                  break;
    default     : writeErrorTX(E_UNKOWN_COMMAND);
                  break;
  }
}


/** @} */ // end of group general

#endif /* GENERAL.H */
