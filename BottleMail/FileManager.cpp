/*
 * FileManager.cpp
 *
 * Created: 15.11.2014 16:46:14
 *  Author: joern
 */

#include "FileManager.h"
// #include "Command.h"
// #include "FrameSender.h"

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

void FileManager::readMessage(const uint16_t &number)
{
  if (!isMessageNumberValid(number))
  {
  }

  const char *filename = getFileName(number);

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

void FileManager::writeMessage(const uint16_t &number)
{
  const char *filename = getFileName(number);
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

  return number <= messageCount + 1;
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
// XModem
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
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
  // Receive file
  if (doWrite)
    return writeFromBufferToFile(no, data);

  // Transmit file
  else
    return readFromFileToBuffer(no, data);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// XModem
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
uint16_t FileManager::messageCount = 0;
const char *FileManager::messageCountFile = "COUNT.TXT";

Fat16 FileManager::file;
bool FileManager::doWrite = false;

XModem FileManager::modem(recvChar, sendChar, dataHandler);
