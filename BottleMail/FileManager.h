/*
 * FileManager.h
 *
 * Created: 15.11.2014 15:56:11
 *  Author: joern
 */


#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <SdFile.h>
#include <stdint.h>
#include "XModem.h"

namespace BottleMail {

class FileManager
{
  public:
    enum error_t
    {
      kErrorNone = 0,
      kErrorNumberInvalid,
      kErrorFileNotFound,
      kErrorWriteIncomplete,
      kErrorSpaceInsufficient,
    };

    // General
    static void initialize();

    // Message Handling
    static error_t readMessage(const uint16_t &number);
    static error_t writeMessage(const uint16_t &number);
    static const uint16_t &getMessageCount();

  private:
    // General
    static void writeInfoFile();

    // Message Count
    static uint16_t readMessageCount();
    static bool writeMessageCount(const uint16_t &count);
    static bool isMessageNumberValid(const uint16_t &number);

    // File I/O
    static const char *getFileName(const uint16_t &number);
    static error_t openFile(const uint16_t &number, bool write);
    static bool writeFromBufferToFile(unsigned long &no, char *data);
    static bool readFromFileToBuffer(unsigned long &no, char *data);

    // XModem Protocol
    static bool sendFile();
    static bool receiveFile();
    static int  recvChar(int msDelay);
    static void sendChar(char sym);
    static bool dataHandler(unsigned long no, char *data, int size);

  private:
    // File Variables
    static SdFile file;

    // Message Variables
    static uint16_t messageCount;
    static const char *messageCountFile;

    // Xmodem Variables
    static bool shouldReceiveFile;
    static XModem modem;
};

}
#endif

