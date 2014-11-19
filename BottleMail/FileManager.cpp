/*
 * FileManager.cpp
 *
 * Created: 15.11.2014 16:46:14
 *  Author: joern
 */

#include "FileManager.h"

using namespace BottleMail;

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// Message Handling
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void FileManager::initialize()
{
  messageCount = readMessageCount();

  if (messageCount == 0)
  {
    writeInfoFile();
    messageCount++;
    writeMessageCount(messageCount);
  }
}

FileManager::error_t FileManager::openFile(const uint16_t &number, bool write)
{
  // Check file number
  if (!isMessageNumberValid(number))
    return kErrorNumberInvalid;

  // Get file name and options to open file
  const char *filename = getFileName(number);
  const uint8_t options = write ? O_CREAT | O_WRITE : O_READ;

  // Open file
  file.open(filename, options);
  if (!file.isOpen())
    return write ? kErrorSpaceInsufficient : kErrorFileNotFound;

  return kErrorNone;
}

FileManager::error_t FileManager::readMessage(const uint16_t &number)
{
  error_t error = openFile(number, false);
  if (error)
    return error;

  sendFile();

  // Close file
  file.close();
  return kErrorNone;
}

FileManager::error_t FileManager::writeMessage(const uint16_t &number)
{
  error_t error = openFile(number, true);
  if (error)
    return error;

  bool wasCompleted = receiveFile();

  if (wasCompleted)
  {
    // Remove tailing bytes if new message is smaller
    file.truncate(file.curPosition());

    // Check if we have a new message and must increase message count
    if (messageCount == number)
    {
      messageCount++;
      writeMessageCount(messageCount);
    }
  }

  else
    file.truncate(0);

  file.close();
  return wasCompleted ? kErrorNone : kErrorWriteIncomplete;
}

const uint16_t &FileManager::getMessageCount()
{
  return messageCount;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// Message Number
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void FileManager::writeInfoFile()
{
  static const char *str = PSTR("{\"firmware\":0.1,\"serial\":1,\"type\":0,\"date\":\"2014-11-16T16:00:00\"}");

  file.open(getFileName(0), O_CREAT | O_WRITE);
  if (file.isOpen())
  {
    file.write(str);
    file.close();
  }
};

uint16_t FileManager::readMessageCount()
{
  // Open file
  file.open(messageCountFile, O_READ);
  if (!file.isOpen())
    return 0;

  // Read count
  uint16_t count;
  int16_t readBytes = file.read(&count, sizeof(count));
  file.close();

  // Return
  if (readBytes != sizeof(count))
    return 0;
  else
    return count;
}

bool FileManager::writeMessageCount(const uint16_t &count)
{
  // Check if count is >0 because, "0.txt" is the bottle information file.
  if (count == 0)
    return false;

  // Open file
  file.open(messageCountFile, O_CREAT | O_WRITE);
  if (!file.isOpen())
    return false;

  // Write count
  int16_t writtenBytes = file.write(&count, sizeof(count));
  file.close();

  // Return
  if (writtenBytes != sizeof(count))
    return false;
  else
    return true;
}

bool FileManager::isMessageNumberValid(const uint16_t &number)
{
  // Number 0 is the configuration file
  if (number == 0)
    return false;

  return number < messageCount + 1;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// File I/O
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
const char *FileManager::getFileName(const uint16_t &filenumber)
{
  // Max. file name would be "65535.TXT" + \0 = 10 chars
  static char name[10];
  static const char *suffix = ".TXT";

  utoa(filenumber, name, 10);
  strcat(name, suffix);

  return name;
}

bool FileManager::readFromFileToBuffer(unsigned long &no, char *data)
{
  int16_t readBytes = file.read(data, 128);

  // End not reached, thus we cannot close transmission
  if (readBytes > 0)
    return true;

  // Cannot read file, thus end transmission
  else
    return false;
}

bool FileManager::writeFromBufferToFile(unsigned long &no, char *data)
{
  int16_t writtenBytes = file.write(data, 128);

  // Bytes written
  if (writtenBytes == 128)
    return true;

  // If something goes wrong, abort connection
  else
    return false;
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// XModem Protocol
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
bool FileManager::receiveFile()
{
  shouldReceiveFile = true;
  return modem.receive();
}

bool FileManager::sendFile()
{
  shouldReceiveFile = false;
  return modem.transmit();
}

void FileManager::sendChar(char sym)
{
  Serial.write(sym);
}

int FileManager::recvChar(int msDelay)
{
  int cnt = 0;
  while (cnt < msDelay)
  {
    if (Serial.available() > 0)
      return Serial.read();
    delay(1);
    cnt++;
  }
  return -1;
}

bool FileManager::dataHandler(unsigned long no, char *data, int size)
{
  if (shouldReceiveFile)
    return writeFromBufferToFile(no, data);

  else
    return readFromFileToBuffer(no, data);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// XModem
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
uint16_t FileManager::messageCount = 0;
const char *FileManager::messageCountFile = "COUNT.TXT";

SdFile FileManager::file;
bool FileManager::shouldReceiveFile = false;

XModem FileManager::modem(recvChar, sendChar, dataHandler);
