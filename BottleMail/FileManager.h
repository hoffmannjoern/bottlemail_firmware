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
    static void initialize();
    static void readMessage(const uint16_t &number);
    static void writeMessage(const uint16_t &number);

  private:
    // Message Number
    static uint16_t readMessageCount();
    static bool writeMessageCount(const uint16_t &count);
    static bool isMessageNumberValid(const uint16_t &number);

    // Message I/O
    static const char *getFileName(const uint16_t &number);
    static bool writeFromBufferToFile(unsigned long &no, char *data);
    static bool readFromFileToBuffer(unsigned long &no, char *data);

    // XModem
    static int  recvChar(int msDelay);
    static void sendChar(char sym);
    static bool dataHandler(unsigned long no, char *data, int size);

  private:
    static uint16_t messageCount;
    static const char *messageCountFile;
    static bool doWrite;
    static Fat16 file;
    static XModem modem;
};



#endif /* FILEMESSAGE_H_ */