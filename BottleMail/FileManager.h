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
      kErrorNumberInvalid,        // 1
      kErrorNotFound,             // 2
      kErrorWriteIncomplete,      // 3
      kErrorSpaceInsufficient,    // 4
      kErrorNotAllowed,           // 5
    };

    // General
    static void initialize();

    // Message Handling
    static error_t select(const uint16_t &number, bool write);
    static error_t read(const uint16_t &number);
    static error_t write(const uint16_t &number);
    static const uint16_t &getCount();

  private:
    // General
    static void writeInfoFile();

    // Message Count
    static uint16_t readMessageCount();
    static bool isMessageNumberValid(const uint16_t &number);
    static bool isMessageAccessable(const uint16_t &number, const bool &write);
    static void setNewMessageCount(const uint16_t &number);
    static bool writeMessageCount(const uint16_t &count);

    // File I/O
    static error_t checkSelectedFile(const uint16_t &number);
    static const char *getFileName(const uint16_t &number);
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
    static uint16_t selectedMessage;
    static uint16_t messageCount;
    static const char *messageCountFile;

    // Xmodem Variables
    static bool shouldReceiveFile;
    static XModem modem;
};

}
#endif

