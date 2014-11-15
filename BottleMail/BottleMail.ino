/*
 * This sketch reads and prints the file
 * PRINT00.TXT created by fat16print.ino or
 * WRITE00.TXT created by fat16write.ino
 */
#include <Fat16.h>
#include <Fat16util.h> // use functions to print strings from flash memory

#include "Stream.h"
#include "Frame.h"
#include "FrameRecognizer.h"
#include "FrameSender.h"
#include "Command.h"
#include "CommandHandler.h"
#include <XModem.h>

const uint8_t CHIP_SELECT = SS;  // SD card chip select pin.
SdCard card;
Fat16 file;
bool write;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

void error_P(const char *str)
{
  Serial.print(F("error: "));
  SerialPrintln_P(str);
  if (card.errorCode)
  {
    Serial.print(F("SD error: "));
    Serial.println(card.errorCode, HEX);
  }
}

/*
 * Write an unsigned number to file.
 * Normally you would use print to format numbers.
 */
void writeNumber(uint32_t n)
{
  uint8_t buf[10];
  uint8_t i = 0;
  do
  {
    i++;
    buf[sizeof(buf) - i] = n % 10 + '0';
    n /= 10;
  }
  while (n);
  file.write(&buf[sizeof(buf) - i], i); // write the part of buf with the number
}


void setup(void)
{
  Serial.begin(38400);

  // Initialize the SD card
  if (!card.begin(CHIP_SELECT))
    error("card.begin");

  // Initialize a FAT16 volume
  if (!Fat16::init(&card))
    error("Fat16::init");
}

void itoa_buffer(uint16_t number, char *buffer, uint8_t buffer_len)
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

int create_file(uint16_t number)
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
    error ("file.open");
    return -1;
  }

  Serial.print(F("Writing to: "));
  Serial.println(file_name);

  // write 100 line to file
  for (uint8_t i = 0; i < 100; i++)
  {
    file.write("line "); // write string from RAM
    writeNumber(i);
    file.write_P(PSTR(" millis = ")); // write string from flash
    writeNumber(millis());
    file.write("\r\n"); // file.println() would work also
  }

  // close file and force write of all data to the SD card
  file.close();
}

int recvChar(int msDelay)
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

void sendChar(char sym)
{
  Serial.write(sym);
}


bool readFromFileToBuffer(unsigned long &no, char *data)
{
  int16_t readBytes = file.read(data, 128);

  // End not reached, thus we cannot close transmission
  if (readBytes > 0)
    return true;

  // Cannot read file, thus end transmission
  else
    return false;
}

bool writeFromBufferToFile(unsigned long &no, char *data)
{
  int16_t writtenBytes = file.write(data, 128);

  // Bytes written
  if (writtenBytes == 128)
    return true;

  // Abort connection, something goes wrong
  else
    return false;
};

bool dataHandler(unsigned long no, char *data, int size)
{

  // Receive file
  if (write)
    return writeFromBufferToFile(no, data);

  // Transmit file
  else
    return readFromFileToBuffer(no, data);

}

const char filename[] = "00100.TXT";
XModem modem(recvChar, sendChar, dataHandler);

using namespace BottleMail;
static CommandHandler commandHandler;
static FrameRecognizer frameRecognizer(commandHandler);

uint8_t receiveByte()
{
  // Wait until byte is received
  while (!Serial.available())
    ;

  return Serial.read();
}


void loop(void)
{
  Serial.println();
  Serial.println(F("Type any character to start"));

  uint8_t byte = receiveByte();

  frameRecognizer.addByte(byte);
  if (frameRecognizer.isProcessing())
    return;

  else if (byte == 'l')
  {
    // Show files
    Serial.println(F("Name          Modify Date/Time    Size"));
    Fat16::ls(LS_DATE | LS_SIZE);
    Serial.println(F("Done"));
  }

  else if (byte == 'z')
  {
    create_file(100);
    Serial.println(F("Done"));
  }

  else if (byte == 'r')
  {
    file.open(filename, O_READ);
    if (!file.isOpen())
    {
      error ("file.open");
      return;
    }

    Serial.println(F("Please start xmodem receiver."));

    write = false;
    modem.transmit();

    file.close();
  }

  else if (byte == 'w')
  {
    file.open(filename, O_CREAT | O_WRITE);
    if (!file.isOpen())
    {
      error ("file.open");
      return;
    }

    Serial.println(F("Pleas start xmodem sender."));

    write = true;
    modem.receive();
    file.truncate(file.curPosition());
    file.close();
  }
}



