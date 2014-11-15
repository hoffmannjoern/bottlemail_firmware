/*
 * Message.cpp
 *
 * Created: 15.11.2014 16:46:14
 *  Author: joern
 */

#include "Message.h"

// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
// Helper
// --------------------------------------------------------------------------------------------------------------------------------------------------------- //
void Message::itoa_buffer(uint16_t number, char *buffer, uint8_t buffer_len)
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
bool   Message::doWrite;
Fat16  Message::file;
char   Message::filename[10] = "00000.TXT";

bool Message::readFromFileToBuffer(unsigned long &no, char *data)
{
  int16_t readBytes = file.read(data, 128);

  // End not reached, thus we cannot close transmission
  if (readBytes > 0)
    return true;

  // Cannot read file, thus end transmission
  else
    return false;
}

bool Message::writeFromBufferToFile(unsigned long &no, char *data)
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
XModem Message::modem = XModem(recvChar, sendChar, dataHandler);

void Message::sendChar(char sym)
{
  Serial.write(sym);
}

int Message::recvChar(int msDelay)
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

bool Message::dataHandler(unsigned long no, char *data, int size)
{

  // Receive file
  if (doWrite)
    return writeFromBufferToFile(no, data);

  // Transmit file
  else
    return readFromFileToBuffer(no, data);
}