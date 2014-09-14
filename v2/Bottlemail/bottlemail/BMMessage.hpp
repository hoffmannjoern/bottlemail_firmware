#ifndef __BMMessage_hpp__
#define __BMMessage_hpp__

#include "Buffer.hpp"

/**
 * @class BMMessage
 * Class to handle a buffer that contains a Bottl(e)mail message.
 *
 * The assumed package format of the buffer is:
 * <cmd(1), len(1), payload(len), crc(2)>
 */
class BMMessage
{
public:
  
  /**
   * Initialize the message by operating on the given buffer.
   * @param buffer The buffer the operations operate on.
   */
  BMMessage(Buffer &buffer);

  /**
   * Returns the setted command of the header.
   */
  uint8_t getCmd();

  /**
   * Sets the command of the header.
   */
  void setCmd(uint8_t cmd);

//--------------------------------------------------------------------------------------------------------------------//
#pragma mark - Payload
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

//--------------------------------------------------------------------------------------------------------------------//
#pragma mark - CRC
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

//--------------------------------------------------------------------------------------------------------------------//
#pragma mark - Size
  /**
   * Returns the overall size in bytes of the message including the bytes needed by the checksum.
   */
  uint16_t getSize();

  /**
   * Returns the overall size in bytes of the message without the bytes needed by the checksum
   */
  uint16_t getSizeWithoutCRC();
  

  // The managed buffer
  Buffer  &_buffer;
  
private:
  uint8_t  _payloadSizeMax;
};

#endif
