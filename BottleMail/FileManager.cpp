/*
 * FileManager.cpp
 *
 * Created: 15.11.2014 16:46:14
 *  Author: joern
 */

#include "FileManager.h"

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// Helper
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void FileManager::itoa_buffer(uint16_t number, char *buffer, uint8_t buffer_len)
{
  uint8_t i = 0;
  do
  {
    i++;
    buffer[buffer_len - i] = number % 10 + '0';
    number /= 10;
  }
  while (number);
}

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// File I/O
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
bool   FileManager::doWrite;
Fat16  FileManager::file;
char   FileManager::filename[10] = "00000.TXT";

void FileManager::readMessage(const uint16_t &number)
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

void FileManager::writeMessage(const uint16_t &number)
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

int FileManager::createFile(const uint16_t &number)
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

  // Abort connection, something goes wrong
  else
    return false;
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// XModem
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
XModem FileManager::modem = XModem(recvChar, sendChar, dataHandler);

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