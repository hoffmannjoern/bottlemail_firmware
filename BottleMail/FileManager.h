/*
 * FileManager.h
 *
 * Created: 15.11.2014 15:56:11
 *  Author: joern
 */


#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <Fat16.h>
#include <XModem.h>

class FileManager
{
  public:
    static void readMessage(const uint16_t &number);
    static void writeMessage(const uint16_t &number);

  private:
    // File iO
    static bool doWrite;
    static Fat16 file;
    static char filename[10]; // = "00000.TXT";

    static int createFile(const uint16_t &number);
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