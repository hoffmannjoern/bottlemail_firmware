#ifndef __BMMessage_hpp__
#define __BMMessage_hpp__

#include "Buffer.hpp"

/*
 * The assumed package format is:
 * <cmd(1), len(1), payload(len), crc(2)>
 */

typedef struct
{
  uint8_t cmd;
  uint8_t payloadSize;
}BMMessageHeader;

class BMMessage
{
public:
  BMMessage(Buffer &buffer);

  /**
   * Returns the setted command of the header.
   */
  uint8_t getCmd();

  /**
   * Sets the command of the header.
   */
  void setCmd(uint8_t cmd);

  /**
   * Returns a pointer to the payload. @see getPayloadSize() or @see getPayloadBuffer()
   */
  void *getPayload();

  /**
   * Returns the current payload size.
   */
  uint8_t getPayloadSize();

  /**
   * Returns the payload as buffer structure.
   */
  Buffer getPayloadBuffer();

  /**
   * Copies the given buffer to the payload.
   * @note The checksum is NOT updated.
   * @return 0 = success, -1 insufficient size
   */
  int writePayload(const void *buffer, uint8_t size);

  /**
   * Appends the given byte to the payload.
   * @note The checksum is NOT updated.
   * @return 0 = success, -1 insufficient size
   */
  int appendPayload(unsigned char byte);
  
  /**
   *  Returns the checksum
   */
  uint16_t getCRC();

  /**
   * Computes and sets checksum
   */
  void setCRC();

  /**
   * Sets a pre-computed checksum
   */
  void setCRC(uint16_t crc);

  /**
   * Computes but not sets the checksum.
   */
  uint16_t computeCRC();

  /**
   * Computes the checksum and checks if it matches the set one.
   * @return true = valid, false = invalid.
   */
  bool isCRCValid();
  
  /**
   * Returns the overall size in bytes of the message including the bytes needed by the checksum.
   */
  uint16_t getSize();

  /**
   * Returns the overall size in bytes of the message without the bytes needed by the checksum
   */
  uint16_t getSizeWithoutCRC();
  
private:
  Buffer  &_buffer;
  uint8_t  _payloadSizeMax;
};

#endif
