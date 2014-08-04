#ifndef WRITE.H
#define WRITE.H

#include "structs.h"

/**
     * \addtogroup	write_messages Write messages
     * 
     * This group uses the ID 02xx.
     * 
     * @{
     */
     
     
     /** \fn writeMessageHeader(validMessage &m)
	 * @param       validMessage Pointer to message with ID_t=0x0201
         * @return      ID_t answer type
	 */
          void writeMessageHeader(validMessage &m) {
/*            if (DEBUG_MODE) {
              Serial.println("\n[memcheck]");
              Serial.println(freeRam());
            }
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // First 64 Bits after ID are supposed to be unix timestamp
            Serial.readBytes((char *)&m.p.mHeader.date, 8);
            //Serial.print("mHeader date: ");
            //swap_uint64(m.p.mHeader.date);
            //writeByte(m.p.mHeader.date);
            //Serial.println();
            if (Serial.available() == 0) delay(SERIAL_DELAY);  
            // Next 8 Bits are amount of UTF-8 characters for authorLength
            m.p.mHeader.authorLength = Serial.read();
            //Serial.print("mHeader authorLength: ");
            //Serial.write((m.p.mHeader.authorLength));
            //Serial.println();
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Next 8 Bits are amount of UTF-8 characters for titleLength
            m.p.mHeader.titleLength = Serial.read();
            //Serial.print("mHeader titleLength: ");
            //Serial.write((m.p.mHeader.titleLength));
            //Serial.println();
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Next 64 Bits are amount of UTF-8 characters for textLength
            Serial.readBytes((char *) &m.p.mHeader.textLength, 8);
            //Serial.print("mHeader textLength: ");
            //swap_uint64(m.p.mHeader.textLength);
            //for (int i=0; i<8; i++)
           //   Serial.write(uint8_t (m.p.mHeader.textLength >> (i*8)));
           // Serial.println();
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Next 64 Bits are amount of UTF-8 characters for pictureLength
            Serial.readBytes((char *) &m.p.mHeader.pictureLength, 8);
            //Serial.print("mHeader pictureLength: ");
            //swap_uint64(m.p.mHeader.pictureLength);
            //for (int i=0; i<8; i++)
            //  Serial.write(uint8_t (m.p.mHeader.pictureLength >> (i*8)));
           // Serial.println();
  
            // Allocate memory for author
            m.p.mHeader.author = new char[m.p.mHeader.authorLength];
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Read author
            Serial.readBytes(m.p.mHeader.author, m.p.mHeader.authorLength);
            if (DEBUG_MODE) {
              Serial.print("mHeader author: ");
              for (int i=0; i<m.p.mHeader.authorLength; i++)
                Serial.write(m.p.mHeader.author[i]);
              Serial.println();
            }
  
            // Allocate memory for title
            m.p.mHeader.title = new char[m.p.mHeader.titleLength];
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Read title
            Serial.readBytes(m.p.mHeader.title, m.p.mHeader.titleLength);
            if (DEBUG_MODE) {
              Serial.print("mHeader title: ");
              for (int i=0; i<m.p.mHeader.titleLength; i++)
                Serial.write(m.p.mHeader.title[i]);
              Serial.println();
            }
            // Next character should be end character
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            char c = Serial.read();
            if (DEBUG_MODE) {
              if (c != '!') Serial.print("hier ist fehler: ");
              Serial.println(c);
            }
            if (Serial.available() < 2) delay(SERIAL_DELAY);
            // Last two character are crc sum
            m.crc16 = (Serial.read()<<8) | (Serial.read());
            // If end character is missing or crc error occurs --> release allocated memory, write errorcode and send message
            if (c != '!' || crcverify(m, sizeof(m.p.mHeader)-8, m.p.mHeader.author, m.p.mHeader.authorLength, m.p.mHeader.title, m.p.mHeader.titleLength) == false) {
              if (DEBUG_MODE) Serial.println("CRC ERROR");

              delete m.p.mHeader.author;
              delete m.p.mHeader.title;
              error(E_CRC_ERROR);
              //m.ID = E_ID;
              //m.p.errorcode = E_CRC_ERROR;
              //crcappend(m, sizeof(m.p.errorcode));
              //writeMessage(m, sizeof(m.p.errorcode)); 
            }
            else {
              // First check if enough space is available for header, text and picture files
              if (freeSpace < (sizeof(m.p.mHeader) + 4 + m.p.mHeader.authorLength+1 + m.p.mHeader.titleLength + swap_uint64(m.p.mHeader.textLength) + swap_uint64(m.p.mHeader.pictureLength))) {
                if (DEBUG_MODE) Serial.println("Not enough Space");

                delete m.p.mHeader.author;
                delete m.p.mHeader.title;
                error(E_NO_SPACE_ON_SD);
                //m.ID = E_ID;
                //m.p.errorcode = E_NO_SPACE_ON_SD;
                //crcappend(m, sizeof(m.p.errorcode));
                //writeMessage(m, sizeof(m.p.errorcode));
              } 
              else {
                // Space is sufficient but it could happen, that text and picture size is zero --> return error
                if (m.p.mHeader.textLength == 0 && m.p.mHeader.pictureLength == 0) {
                  if (DEBUG_MODE) Serial.println("Textlength and piclength == 0");
                  
                  delete m.p.mHeader.author;
                  delete m.p.mHeader.title;
                  error(E_EMPTY_DATA);
                  //m.ID = E_ID;
                  //m.p.errorcode = E_EMPTY_DATA;
                  //crcappend(m, sizeof(m.p.errorcode));
                  //writeMessage(m, sizeof(m.p.errorcode));
                } 
                else {
                  if (DEBUG_MODE) Serial.println("free Space");

                  // Space is sufficient for header and text or picture files have size greater 0
                  // Create new directory for header file, text file and picture file with next available index (HEX)
                  char buf[8] = "";
                  ltoa(++ini.index, buf, 16);
                  SdFile file, dir;
      
                  if (DEBUG_MODE) {
                    Serial.println("\n[memcheck]");
                    Serial.println(freeRam());
                  }
        
                  dir.makeDir(&root, buf);
                  dir.open(&root, buf, O_RDONLY);
                  file.open(&dir, "header", O_CREAT | O_RDWR);
                  char *p = (char *) &m.p.mHeader;
                  if (DEBUG_MODE) {
                    for (int j=0; j < sizeof(m.p.mHeader) - 8; j++)
                      Serial.write(p[j]);
                    }
                  // Write all information to header file
                  file.write((void *)&m.p.mHeader, sizeof(m.p.mHeader) - 10);
                  file.sync();
                  file.write((void *)m.p.mHeader.author, m.p.mHeader.authorLength);
                  file.sync();
                  file.write((void *)m.p.mHeader.title, m.p.mHeader.titleLength);
                  file.sync();
                  file.close();
                  // Release memory for author and title
                  delete m.p.mHeader.author;
                  delete m.p.mHeader.title;
                  // Save new index in config.ini file
                  file.open(&root, "config.ini", O_RDWR);
                  file.write((void *) &ini.index, sizeof(ini));
                  file.close();
                  // Set flag to indicate if text/picture shall be written with next command
                  textLeft = swap_uint64 (m.p.mHeader.textLength);
                  pictureLeft = swap_uint64 (m.p.mHeader.pictureLength);

                  root.ls(LS_R | LS_DATE | LS_SIZE);
                  //dir.ls(LS_R | LS_DATE | LS_SIZE);
                  dir.close();
                    
                  // Write message - everything seems to be OK
                  crcappend(m, 0);
                  writeMessage(m, 0);
                }
              }
            }                     */
          }


        /** \fn writeMessageTextBlock(validMessage &m)
	 *  @param 	validMessage Pointer to message with ID_t=0x0202
	 *  @return	ID_t	 		answer type
	 */
          void writeMessageTextBlock(validMessage &m) {
          /*  uint16_t t = min(textLeft, MSG_SIZE);

            //Serial.println();
            m.p.text.text = new char[t];
  
            if (DEBUG_MODE) {
              Serial.print("t=");
              Serial.println(t);
            }
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Read from serial buffer until maximum length or timeout
            Serial.readBytes (m.p.text.text, t);
  
            // Next character needs to be end character
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            char c = Serial.read();
            if (DEBUG_MODE) {
              Serial.print("c=");
              Serial.println(c);
            }
            // Next two Bytes should be crc sum
            if (Serial.available() < 2) delay(SERIAL_DELAY);
            m.crc16 = (Serial.read()<<8) | (Serial.read());

            // Check end character and crc sum --> in case of error: release memory for text and send errorcode
            if (c != '!' || crcverify(m, 8, m.p.text.text, t, m.p.text.text, 0) == false){
              delete m.p.text.text;
              error(E_CRC_ERROR);
              //m.ID = E_ID;
              //m.p.errorcode = E_CRC_ERROR;
              //crcappend(m, sizeof(m.p.errorcode));
              //writeMessage(m, sizeof(m.p.errorcode));
            } 
            else {
              char buf[8] = "";
              ltoa(ini.index, buf, 16);
              SdFile dir, file;
              dir.open(&root, buf, O_RDONLY);
              file.open(&dir, "text", O_CREAT | O_APPEND | O_RDWR);
              // Write message to text file
              file.write(m.p.text.text, t);
              file.sync();
              file.close();
    
              // Update textLeft
              textLeft -= t;
              // Release text memory
              delete m.p.text.text;
              // Write OK message
              crcappend(m, 0);
              writeMessage(m, 0);
            }        */
          }


        /** \fn writeMessagePictureBlock(validMessage &m)
	 *  @param 	  validMessage Pointer to message with ID_t=0x0203
	 */
          void writeMessagePictureBlock(validMessage &m) {

          /*  uint16_t t = min(pictureLeft, MSG_SIZE);

            //Serial.println();
            m.p.text.text = new char[t];
  
            if (DEBUG_MODE) {
              Serial.print("t=");
              Serial.println(t);
            }
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            // Read from serial buffer until maximum length or timeout
            Serial.readBytes (m.p.pic.picture, t);
  
            // Next character needs to be end character
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            char c = Serial.read();
            if (DEBUG_MODE) {
              Serial.print("c=");
              Serial.println(c);
            }
            // Next two Bytes should be crc sum
            if (Serial.available() < 2) delay(SERIAL_DELAY);
            m.crc16 = (Serial.read()<<8) | (Serial.read());

            // Check end character and crc sum --> in case of error: release memory for text and send errorcode
            if (c != '!' || crcverify(m, 8, m.p.pic.picture, t, m.p.pic.picture, 0) == false){
              delete m.p.pic.picture;
              error(E_CRC_ERROR);
              //m.ID = E_ID;
              //m.p.errorcode = E_CRC_ERROR;
              //crcappend(m, sizeof(m.p.errorcode));
              //writeMessage(m, sizeof(m.p.errorcode));
            } 
            else {
              char buf[8] = "";
              ltoa(ini.index, buf, 16);
              SdFile dir, file;
              dir.open(&root, buf, O_RDONLY);
              file.open(&dir, "pic", O_CREAT | O_APPEND | O_RDWR);
              // Write message to text file
              file.write(m.p.pic.picture, t);
              file.sync();
              file.close();
    
              // Update textLeft
              pictureLeft -= t;
              // Release text memory
              delete m.p.pic.picture;
              // Write OK message
              crcappend(m, 0);
              writeMessage(m, 0);
            }    */
          }
          
          
          
          
        /** \fn deleteLastIndex()
	 */
          void deleteLastIndex() {
          /*  char buf[8] = "";
            ltoa(ini.index--, buf, 16);
            SdFile file, dir;
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
            file.close(); */
          }

        /** \fn write(validMessage &m)
	 *  @param 	  validMessage Pointer to message with ID_t=0x02XX
         *  @return	  ID_t	 		answer type
	 */
          void write(validMessage &m) {
            do {
              switch (uint8_t (m.ID)){
                case(0x01): //Serial.println("writeMessageHeader");
                            if (textLeft != 0 || pictureLeft != 0) {   
                              error(E_LAST_MESSAGE_INCOMPLETE);                           
                              /*m.ID = E_ID;
                              m.p.errorcode=E_LAST_MESSAGE_INCOMPLETE;
                              crcappend(m, sizeof(m.p.errorcode));
                              writeMessage(m, sizeof(m.p.errorcode));*/
                            } 
                            else
                              writeMessageHeader(m);
                            break;
                case(0x02): //Serial.println("writeMessageTextBlock");
                            if (textLeft == 0) {
                              error(E_TEXT_DATA_SIZE);
                              /*m.ID = E_ID;
                              m.p.errorcode=E_TEXT_DATA_SIZE;
                              crcappend(m, sizeof(m.p.errorcode));
                              writeMessage(m, sizeof(m.p.errorcode));*/
                            } 
                            else {
                              writeMessageTextBlock(m);
                            }
                            break;
                case(0x03): //Serial.println("writeMessagePictureBlock");
                            if (pictureLeft == 0) {
                              error(E_PIC_DATA_SIZE);
                              /*m.ID = E_ID;
                              m.p.errorcode=E_PIC_DATA_SIZE;
                              crcappend(m, sizeof(m.p.errorcode));
                              writeMessage(m, sizeof(m.p.errorcode));*/
                            } 
                            else {
                              writeMessagePictureBlock(m);
                            }
                            break;
                default:    
                            error(E_UNKOWN_COMMAND);
                            /*m.ID = E_ID;
                            m.p.errorcode=E_UNKOWN_COMMAND;
                            crcappend(m, sizeof(m.p.errorcode));
                            writeMessage(m, sizeof(m.p.errorcode));*/
                            break;
              }

              if (textLeft == 0 && pictureLeft == 0) break;
              // Wait for incoming start character
              while(Serial.read() != '$'){
                if (Serial.available() == 0) delay(SERIAL_DELAY);
              }
              if (Serial.available() < 2) delay(SERIAL_DELAY);
              // Read ID
              m.ID = (Serial.read() << 8) | Serial.read();
                if (uint8_t (m.ID>>8) != 0x02) {
                  if (DEBUG_MODE) Serial.println("falscher Code");

                  //m.ID = E_ID;
                  if (pictureLeft != 0) //m.p.errorcode=E_PIC_DATA_SIZE;
                    error(E_PIC_DATA_SIZE);
                  if (textLeft != 0) //m.p.errorcode=E_TEXT_DATA_SIZE;
                    error(E_TEXT_DATA_SIZE);
                  //crcappend(m, sizeof(m.p.errorcode));
                  writeByte(m.p.errorcode);
                  //writeMessage(m, sizeof(m.p.errorcode));
                  deleteLastIndex();
                  textLeft = 0;
                  pictureLeft = 0;
                }
            } while (textLeft != 0 || pictureLeft != 0);
            textLeft = 0;
            pictureLeft = 0;
          }

/** @} */ // end of group write_messages

#endif /* WRITE.H */

