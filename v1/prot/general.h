#ifndef GENERAL.H
#define GENERAL

#include "structs.h"
#include "crc.h"

void error(uint16_t errorcode) {
      m.ID = E_ID;
      m.p.errorcode = errorcode;
      crcappend(m, sizeof(m.p.errorcode));
      writeMessage(m, sizeof(m.p.errorcode));
  }


/**
     * \addtogroup	general General information
     * 
     * This group uses the ID 00xx.
     * 
     * @{
     */

	/** \fn readProtocolVersion(validMessage &m)
	 *  @param	validMessage Pointer to message
	 *  @return	protocolVersion in major and minor byte to param union
	 */
          void readProtocolVersion(validMessage &m) {
            m.p.pV.major = EEPROM.read(4);
            m.p.pV.minor = EEPROM.read(5);
          }

        /** \fn readBottleID(validMessage &m)
	 *  @param	validMessage Pointer to message
	 *  @return	bottleID to param union
	 */
          void readBottleID(validMessage &m) {
            m.p.bID.bottleID = (EEPROM.read(0)<<24) | (EEPROM.read(1)<<16) | (EEPROM.read(2)<<8) | EEPROM.read(3);
          }


        /** \fn general(validMessage &m)
	 *  @param	validMessage Pointer to message
	 *  @return	calls sub-functions and writes the message to serial bluetooth
	 */
          void general(validMessage &m) {
            delay(SERIAL_DELAY);
            // All general information have an empty parameter list, that is why the next byte must be the end character
            char c = Serial.read();
            delay(SERIAL_DELAY);
            // Next two bytes after end character must be crc16 value
            m.crc16 = (Serial.read()<<8) | (Serial.read());
            // Check crc
            if (c != '!' || crcverify(m, 0) == false) {
              error(E_CRC_ERROR);
              /*m.ID = E_ID;
              m.p.errorcode = E_CRC_ERROR;
              crcappend(m, sizeof(m.p.errorcode));
              writeMessage(m, sizeof(m.p.errorcode));*/
            }
            else {
              // Check lower byte of ID. If ID is valid, jump to correct method and append crc sum afterwards
              switch (uint8_t (m.ID)){
                  case(0x01): if (DEBUG_MODE) Serial.println("Protocol Version");
                              // CRC for 0x0001: 11001101 11010111 (CD D7)
                              readProtocolVersion(m);
                              crcappend(m, sizeof(m.p.pV));
                              writeMessage(m, sizeof(m.p.pV));
                              //Serial.print("protocol Version:");
                              //Serial.write(m.p.pV.major);
                              //Serial.write(m.p.pV.minor);
                              break;
                  case(0x02): if (DEBUG_MODE) Serial.println("ReadBottleID");
                              // CRC for 0x0002: 10100101 11111101 (A5 FD)
                              readBottleID(m);
                              crcappend(m, sizeof(m.p.bID));
                              writeMessage(m, sizeof(m.p.bID));
                              //writeByte(m.p.bID.bottleID);
                              break;
                  default:    //(7D E4)  
                              error(E_UNKOWN_COMMAND);                  
                              /*m.ID = E_ID;
                              m.p.errorcode=E_UNKOWN_COMMAND;
                              crcappend(m, sizeof(m.p.errorcode));
                              writeMessage(m, sizeof(m.p.errorcode));*/
                              //writeByte(m.p.errorcode);
                              break;
              }
            }
          }
          
          
          
              
/** @} */ // end of group general

#endif /* GENERAL.H */

