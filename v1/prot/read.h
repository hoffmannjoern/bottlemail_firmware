#ifndef READ.H
#define READ.H

#include "structs.h"


 /**
     * \addtogroup	read_messages Read messages
     * 
     * @{
     */


    /** \fn readINI()
     * @return   bool    answer type
     */
      bool readINI() {
        SdFile file;
        // Try to open *.ini file
        if (!file.open(&root, "config.ini", O_READ)) {
          if (DEBUG_MODE) Serial.println("Error: cannot open *.ini");
          return false;
        } 
        // Try to read *.ini information
        //char buf[sizeof(ini)];
        
        char buf[4];
        file.read((void *) buf, 4);
        
        //ini.index = (file.read()<<24 | file.read()<<16 | file.read()<<8 | file.read());
        //if (file.read((void *)&ini.index, sizeof(ini)) < sizeof(ini)) {
        //  if (DEBUG_MODE) Serial.println("\tError: *.ini file corrupt");
        //  return false;
        //}
        
        // Save index information
        ini.index = ((uint32_t (buf[3]))<<24) | ((uint32_t (buf[2]))<<16) | ((uint32_t (buf[1]))<<8) | (uint32_t (buf[0]));
        
        //ini.index = (uint32_t)((((uint8_t) file.read())<<24) | (((uint8_t) file.read())<<16) | (((uint8_t) file.read())<<8) | ((uint8_t) file.read()));
        
        if (DEBUG_MODE) Serial.print("index = ");
        if (DEBUG_MODE) Serial.println(ini.index);
        file.close();
        return true;
      }
        


        /** \fn readNumberOfMessages(validMessage &m)
	 *  @param	validMessage Pointer to message with ID_t=0x0101
	 *  @return	number of messages to param union
	 */
          void readNumberOfMessages(validMessage &m) {
            // There should be no more parameters after ID
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            char c = Serial.read();
            // Next two bytes after end character must be crc16 value
            if (Serial.available() < 2) delay(SERIAL_DELAY);
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
              m.p.numbOfMessages.number = ini.index;
              if (DEBUG_MODE) { Serial.print("number of messages = "); Serial.println(m.p.numbOfMessages.number); }
              crcappend(m, sizeof(m.p.numbOfMessages));
              writeMessage(m, sizeof(m.p.numbOfMessages));
            }
          }


	/** \fn readMessageHeader(validMessage &m)
	 *  @param 	validMessage Pointer to message with ID_t=0x0102
	 *  @return	messageHeader without messageID field
	 */
          void readMessageHeader(validMessage &m) {
            
            delay(SERIAL_DELAY);
            m.p.mHeader.messageID = (((uint32_t)Serial.read())<<24) | (((uint32_t)Serial.read())<<16) | (((uint32_t)Serial.read())<<8) | (((uint32_t)Serial.read()));
            char c = Serial.read(); // Should be end frame
            if (Serial.available() < 2) delay(SERIAL_DELAY);
            m.crc16 = (Serial.read()<<8) | (Serial.read());
            if (c != '!' || crcverify(m, sizeof(m.p.mHeader.messageID)) == false) {
              if (DEBUG_MODE) Serial.println("CRC ERROR");
              error(E_CRC_ERROR);
              /*m.ID = E_ID;
              m.p.errorcode = E_CRC_ERROR;
              crcappend(m, sizeof(m.p.errorcode));
              writeMessage(m, sizeof(m.p.errorcode));*/ 
            }
            else {
            if (DEBUG_MODE) { Serial.print("Message ID: "); Serial.println((long int)m.p.mHeader.messageID); }
            char buf[8] = "";
            ltoa(m.p.mHeader.messageID, buf, 16);
            if (DEBUG_MODE) { Serial.print("buffer: "); Serial.println(buf); }
            SdFile dir, file;
            
            if (!dir.open(&root, buf, O_RDONLY)) {
              if (DEBUG_MODE) Serial.println("Error: Message ID not found.");
              error(E_ID_ERROR);
              /*m.ID = E_ID;
              m.p.errorcode = E_ID_ERROR;
              crcappend(m, sizeof(m.p.errorcode));
              writeMessage(m, sizeof(m.p.errorcode));*/
            } 
            else {
              if (!file.open(&dir, "header", O_RDONLY)) {
                if (DEBUG_MODE) Serial.println("Error: Header not found.");
                error(E_ID_ERROR);
                /*m.ID = E_ID;
                m.p.errorcode = E_ID_ERROR;
                crcappend(m, sizeof(m.p.errorcode));
                writeMessage(m, sizeof(m.p.errorcode));*/
              }
             else {
               
               file.read((void *) buf, 8);
               m.p.mHeader.date = ((uint64_t (buf[7]))<<56) | ((uint64_t (buf[6]))<<48) | ((uint64_t (buf[5]))<<40) | ((uint64_t (buf[4]))<<32)  | ((uint64_t (buf[3]))<<24) | ((uint64_t (buf[2]))<<16) | ((uint64_t (buf[1]))<<8) | (uint64_t (buf[0]));
               if (DEBUG_MODE) { Serial.print("Date: "); Serial.println((long int)m.p.mHeader.date); }
               m.p.mHeader.authorLength = (uint8_t) file.read();
               if (DEBUG_MODE) { Serial.print("Author Length: "); Serial.println(m.p.mHeader.authorLength); }
               m.p.mHeader.titleLength = (uint8_t) file.read();
               if (DEBUG_MODE) { Serial.print("Title Length: "); Serial.println(m.p.mHeader.titleLength); }
               file.read((void *) buf, 8);
               m.p.mHeader.textLength = ((uint64_t (buf[7]))<<56) | ((uint64_t (buf[6]))<<48) | ((uint64_t (buf[5]))<<40) | ((uint64_t (buf[4]))<<32)  | ((uint64_t (buf[3]))<<24) | ((uint64_t (buf[2]))<<16) | ((uint64_t (buf[1]))<<8) | (uint64_t (buf[0]));
               //if (DEBUG_MODE) { Serial.print("Text Length: "); Serial.println((char *)m.p.mHeader.textLength); }
               file.read((void *) buf, 8);
               m.p.mHeader.pictureLength = ((uint64_t (buf[7]))<<56) | ((uint64_t (buf[6]))<<48) | ((uint64_t (buf[5]))<<40) | ((uint64_t (buf[4]))<<32)  | ((uint64_t (buf[3]))<<24) | ((uint64_t (buf[2]))<<16) | ((uint64_t (buf[1]))<<8) | (uint64_t (buf[0]));
               //if (DEBUG_MODE) { Serial.print("Picture Length: "); Serial.println((char *)m.p.mHeader.pictureLength); }
               
               if (m.p.mHeader.authorLength == 0) {
                 m.p.mHeader.author = "";  
               }
               else {
                 /*char authorBuffer[m.p.mHeader.authorLength];
                 for (int i = 0; i < m.p.mHeader.authorLength; i++) {
                   authorBuffer[i] = (uint8_t) file.read();
                 }
                 m.p.mHeader.author = authorBuffer;
                 if (DEBUG_MODE) { Serial.print("Author: "); Serial.println((char *)m.p.mHeader.author); }
                 //delete authorBuffer;
                 authorBuffer <- NULL;*/
                 m.p.mHeader.author=new char[m.p.mHeader.authorLength];
                 file.read((void *)m.p.mHeader.author, m.p.mHeader.authorLength);
                 //if (DEBUG_MODE) { Serial.print("Author: "); Serial.println((char *)m.p.mHeader.author); }
               }
               
               if (m.p.mHeader.titleLength == 0) {
                 m.p.mHeader.title = "";    
               }
               else {
                 /*char titleBuffer[m.p.mHeader.titleLength];
                 for (int i = 0; i < m.p.mHeader.titleLength; i++) {
                   titleBuffer[i] = (uint8_t) file.read();
                 }
                 m.p.mHeader.title = titleBuffer;
                 if (DEBUG_MODE) { Serial.print("Title: "); Serial.println((char *)m.p.mHeader.title); }
                 //delete titleBuffer;
                 titleBuffer <- NULL;*/
                 m.p.mHeader.title=new char[m.p.mHeader.titleLength];
                 file.read((void *)m.p.mHeader.title, m.p.mHeader.titleLength);
                 //if (DEBUG_MODE) { Serial.print("Title: "); Serial.println((char *)m.p.mHeader.title); }
               }
               
               crcappend(m, sizeof(m.p.mHeader) - 4, m.p.mHeader.author, m.p.mHeader.authorLength, m.p.mHeader.title, m.p.mHeader.titleLength);
               writeMessage(m, sizeof(m.p.mHeader) - 4, m.p.mHeader.author, m.p.mHeader.authorLength, m.p.mHeader.title, m.p.mHeader.titleLength);
               
               m.p.mHeader.title <- NULL;
               m.p.mHeader.author <- NULL;
             }
             } 
            file.close(); 
            }
                 
          }


        /** \fn readMessageTextBlock(validMessage &m)
	 *  @param 	validMessage Pointer to message with ID_t=0x0103
         *  @param      uint32_t blockcount (beginning with 0)
	 *  @return	messageText
	 */
          void readMessageTextBlock(validMessage &m) {
            delay(SERIAL_DELAY);
            uint32_t msgID;
            msgID = (((uint32_t)Serial.read())<<24) | (((uint32_t)Serial.read())<<16) | (((uint32_t)Serial.read())<<8) | (((uint32_t)Serial.read()));
            if (DEBUG_MODE) { Serial.print("Message ID: "); Serial.println(msgID); }
            while (Serial.available() < 4) delay(SERIAL_DELAY);
            m.p.data.blockcount = (((uint32_t)Serial.read())<<24) | (((uint32_t)Serial.read())<<16) | (((uint32_t)Serial.read())<<8) | (((uint32_t)Serial.read()));
            if (DEBUG_MODE) { Serial.print("Blockcount: "); Serial.println(m.p.data.blockcount); }
            
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            char c = Serial.read(); // Should be end frame
            if (Serial.available() < 2) delay(SERIAL_DELAY);
            m.crc16 = (Serial.read()<<8) | (Serial.read());
            if (c != '!' || crcverify(m, 12) == false) {
              if (DEBUG_MODE) Serial.println("CRC ERROR");
              error(E_CRC_ERROR);
            }
            else {
              char buf[8] = "";
              ltoa(msgID, buf, 16);
              SdFile dir, file;
              if (!dir.open(&root, buf, O_RDONLY)) {
                if (DEBUG_MODE) Serial.println("Error: Message ID not found.");
                error(E_ID_ERROR);
              }
              else {
                if (DEBUG_MODE) Serial.println("Message ID found!");
                if (!file.open(&dir, "text", O_RDONLY)) {
                  if (DEBUG_MODE) Serial.println("Error: Text not found.");
                  error(E_ID_ERROR);
                }
                else {
                  if (DEBUG_MODE) Serial.println("Text found!");
                  int i; 
                   
                  for (i = m.p.data.blockcount; i > 0; i--) {
                    file.read((void *)m.p.data.text, MSG_SIZE);
                  }
                  m.p.data.text <- NULL;
                  
                   for (i = 0; i < MSG_SIZE; i++) {
                     if (file.available() > 0) { 
                       m.p.data.text[i] = file.read(); 
                       if (DEBUG_MODE) Serial.print(m.p.data.text[i]); 
                     }
                     else { break; }
                   }
                   if (DEBUG_MODE) Serial.println();
                   if (DEBUG_MODE) { Serial.print("i = "); Serial.println(i); }
                   
                   crcappend(m, 4, m.p.data.text, i, 0, 0);
                   writeMessage(m, 4, m.p.data.text, i, 0, 0);
                 
                 if (DEBUG_MODE) Serial.println();
                 if (DEBUG_MODE) Serial.println("Text complete");
                 file.close();  
                }
              }
            }
            
          }


        /** \fn readMessagePictureBlock(validMessage &m)
	 *  @param 	validMessage Pointer to message with ID_t=0x0104
         *  @param      uint32_t blockcount (beginning with 0)
	 *  @return	messagePicture
	 */
          void readMessagePictureBlock(validMessage &m) {
          delay(SERIAL_DELAY);
            uint32_t msgID;
            msgID = (((uint32_t)Serial.read())<<24) | (((uint32_t)Serial.read())<<16) | (((uint32_t)Serial.read())<<8) | (((uint32_t)Serial.read()));
            if (DEBUG_MODE) { Serial.print("Message ID: "); Serial.println(msgID); }
            while (Serial.available() < 4) delay(SERIAL_DELAY);
            m.p.data.blockcount = (((uint32_t)Serial.read())<<24) | (((uint32_t)Serial.read())<<16) | (((uint32_t)Serial.read())<<8) | (((uint32_t)Serial.read()));
            if (DEBUG_MODE) { Serial.print("Blockcount: "); Serial.println(m.p.data.blockcount); }
            
            if (Serial.available() == 0) delay(SERIAL_DELAY);
            char c = Serial.read(); // Should be end frame
            if (Serial.available() < 2) delay(SERIAL_DELAY);
            m.crc16 = (Serial.read()<<8) | (Serial.read());
            if (c != '!' || crcverify(m, 12) == false) {
              if (DEBUG_MODE) Serial.println("CRC ERROR");
              error(E_CRC_ERROR);
            }
            else {
              char buf[8] = "";
              ltoa(msgID, buf, 16);
              SdFile dir, file;
              if (!dir.open(&root, buf, O_RDONLY)) {
                if (DEBUG_MODE) Serial.println("Error: Message ID not found.");
                error(E_ID_ERROR);
              }
              else {
                if (DEBUG_MODE) Serial.println("Message ID found!");
                if (!file.open(&dir, "pic", O_RDONLY)) {
                  if (DEBUG_MODE) Serial.println("Error: Text not found.");
                  error(E_ID_ERROR);
                }
                else {
                  if (DEBUG_MODE) Serial.println("Picture found!");
                  int i; 
                   
                  for (i = m.p.data.blockcount; i > 0; i--) {
                    file.read((void *)m.p.data.pic, MSG_SIZE);
                  }
                  m.p.data.pic <- NULL;
                  
                   for (i = 0; i < MSG_SIZE; i++) {
                     if (file.available() > 0) { 
                       m.p.data.pic[i] = file.read(); 
                       if (DEBUG_MODE) Serial.print(m.p.data.pic[i]); 
                     }
                     else { break; }
                   }
                   if (DEBUG_MODE) Serial.println();
                   if (DEBUG_MODE) { Serial.print("i = "); Serial.println(i); }
                   
                   crcappend(m, 4, m.p.data.pic, i, 0, 0);
                   writeMessage(m, 4, m.p.data.pic, i, 0, 0);
                 
                 if (DEBUG_MODE) Serial.println();
                 if (DEBUG_MODE) Serial.println("Picture complete");
                 file.close();  
                }
              }
            }
        }
        
        
        /** \fn read(validMessage &m)
	 *  @param 	validMessage Pointer to message with ID_t=0x01XX
	 *  @return	calls sub-functions and writes the message to serial bluetooth
	 */
          void read (validMessage &m) {
            switch (uint8_t (m.ID)){
              case(0x01): //Serial.println("readNumberOfMessages"); // (11 8D)
                readNumberOfMessages(m);
                break;
              case(0x02): //Serial.println("readMessageHeader");
                readMessageHeader(m);
                break;
              case(0x03): //Serial.println("readMessageText");
                readMessageTextBlock(m);
                break;
              case(0x04): //Serial.println("readMessagePicture");
                readMessagePictureBlock(m);
                break;
              default:      //Serial.println("unkownCommand");
                error(E_UNKOWN_COMMAND);
                break;
            }
          }

    /** @} */ // end of group read_messages

#endif /* READ.H */


