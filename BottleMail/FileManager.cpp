/*
 * FileManager.cpp
 *
 * Created: 15.11.2014 16:46:14
 *  Author: joern
 */

#include "FileManager.h"

using namespace BottleMail;

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// General
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void FileManager::initialize()
{
  messageCount = readMessageCount();

  if (messageCount == 0)
  {
    writeInfoFile();
    setNewMessageCount(messageCount + 1);
  }
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// Message Handling
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
FileManager::error_t FileManager::select(const uint16_t &number, bool write)
{
  // Close previous file
  if (file.isOpen())
    file.close();

  // Check message number and operation
  if (!isMessageNumberValid(number))
    return kErrorNumberInvalid;

  else if (!isMessageAccessable(number, write))
    return kErrorNotAllowed;

  // Get file name and options to open file
  const char *filename = getFileName(number);
  const uint8_t options = write ? O_CREAT | O_WRITE : O_READ;

  // Open file
  file.open(filename, options);
  if (!file.isOpen())
    return write ? kErrorSpaceInsufficient : kErrorNotFound;

  selectedMessage = number;
  return kErrorNone;
}

FileManager::error_t FileManager::read(const uint16_t &number)
{
  error_t error = checkSelectedFile(number);
  if (error)
    return error;

  sendFile();

  // Close file
  file.close();
  return kErrorNone;
}

FileManager::error_t FileManager::write(const uint16_t &number)
{
  error_t error = checkSelectedFile(number);
  if (error)
    return error;

  bool wasCompleted = receiveFile();
  if (wasCompleted)
  {
    // Remove tailing bytes if new message is smaller
    file.truncate(file.curPosition());

    // Set new message count (if it is really a new message)
    setNewMessageCount(selectedMessage);
  }

  else
    file.truncate(0);

  file.close();
  return wasCompleted ? kErrorNone : kErrorWriteIncomplete;
}


const uint16_t &FileManager::getCount()
{
  return messageCount;
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// General
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void FileManager::writeInfoFile()
{
  static const char *str = PSTR("{\"firmware\":0.1,\"serial\":1,\"type\":0,\"date\":\"2014-11-16T16:00:00\"}");

  file.open(getFileName(0), O_CREAT | O_WRITE);
  if (file.isOpen())
  {
    file.write(str);
    file.truncate(file.curPosition());
    file.close();
  }
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// Message Count
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void FileManager::setNewMessageCount(const uint16_t &number)
{
  // Check if we have a new message and must increase message count
  if (messageCount == number)
  {
    uint16_t newMessageCount = messageCount + 1;
    if (writeMessageCount(newMessageCount))
      messageCount = newMessageCount;
  }
}

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
  return number < messageCount + 1;
}

bool FileManager::isMessageAccessable(const uint16_t &number, const bool &write)
{
  // Dismiss to write the config file
  if (write && number == 0)
    return false;

  return true;
}


// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// File I/O
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
FileManager::error_t FileManager::checkSelectedFile(const uint16_t &number)
{
  if (!file.isOpen())
    return kErrorNotFound;

  else if (number != selectedMessage)
    return kErrorNumberInvalid;

  return kErrorNone;
}

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
// Variables
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// File Variables
SdFile FileManager::file;

// Message Variables
uint16_t FileManager::selectedMessage = 0;
uint16_t FileManager::messageCount = 0;
const char *FileManager::messageCountFile = "COUNT.TXT";

// XModem Variables
bool FileManager::shouldReceiveFile = false;
XModem FileManager::modem(recvChar, sendChar, dataHandler);

