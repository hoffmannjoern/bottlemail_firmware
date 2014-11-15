/*
 * Message.h
 *
 * Created: 15.11.2014 15:56:11
 *  Author: joern
 */


#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <Fat16.h>
#include <XModem.h>

class Message
{
  public:

    void readMessage()
    {
      file.open(filename, O_READ);
      if (!file.isOpen())
      {
        // error ("file.open");
        return;
      }

      Serial.println(F("Please start xmodem receiver."));

      doWrite = false;
      modem.transmit();

      file.close();
    }

    void writeMessage()
    {
      file.open(filename, O_CREAT | O_WRITE);
      if (!file.isOpen())
      {
        // error ("file.open");
        return;
      }

      Serial.println(F("Pleas start xmodem sender."));

      doWrite = true;
      modem.receive();
      file.truncate(file.curPosition());
      file.close();
    }


  private:
    int createFile(uint16_t number)
    {
      char file_name[] = "00000.txt";
      itoa_buffer(number, file_name, 5);
      Serial.println(file_name);

      // O_CREAT - create the file if it does not exist
      // O_EXCL - fail if the file exists
      // O_WRITE - open for write
      file.open(file_name, O_CREAT | O_WRITE);
      if (!file.isOpen())
      {
        // error ("file.open");
        return -1;
      }

      Serial.print(F("Writing to: "));
      Serial.println(file_name);

      // write 100 line to file
      for (uint8_t i = 0; i < 100; i++)
      {
        file.write("line "); // write string from RAM
        // writeNumber(i);

        file.write_P(PSTR(" millis = ")); // write string from flash

        // writeNumber(millis());
        file.write("\r\n"); // file.println() would work also
      }

      // close file and force write of all data to the SD card
      file.close();
    }


  private:
    // File iO
    static bool doWrite;
    static Fat16 file;
    static char filename[10]; // = "00000.TXT";

    static void itoa_buffer(uint16_t number, char *buffer, uint8_t buffer_len);
    static bool writeFromBufferToFile(unsigned long &no, char *data);
    static bool readFromFileToBuffer(unsigned long &no, char *data);

    // XModem
    static XModem modem;
    static int  recvChar(int msDelay);
    static void sendChar(char sym);
    static bool dataHandler(unsigned long no, char *data, int size);
};



#endif /* FILEMESSAGE_H_ */