// MyTestSuite1.h

#include "cxxtest/TestSuite.h"
#include "BMMessage.h"
#include "Buffer.h"
#include <string.h>

class BMMessageTest : public CxxTest::TestSuite
{
  
public:
  void testCreate(void)
  {
    const uint16_t cBufferSize = 64;
    uint8_t cBuffer[cBufferSize] = {'a', 6, 'H', 'e', 'l', 'l', 'o', 0, 0xe4, 0x57};

    Buffer buffer(cBuffer, cBufferSize);
    BMMessage msg = BMMessage(buffer);
    
    // Check if buffer is read correctly
    TS_ASSERT_EQUALS(msg.getCmd(), 'a');
    TS_ASSERT_EQUALS(msg.getPayloadSize(), 6);
    TS_ASSERT_EQUALS(msg.getSizeWithoutCRC(), 8);
    TS_ASSERT_EQUALS(msg.getSize(), 10);
    TS_ASSERT( strcmp((char*)msg.getPayload(), "Hello") == 0 );
    TS_ASSERT_EQUALS(msg.isCRCValid(), true);
  }
  
  void testCRC(void)
  {
    const uint16_t cBufferSize = 64;
    uint8_t cBuffer[cBufferSize] = {'a', 6, 'H', 'e', 'l', 'l', 'o', 0, 0xe4, 0x57};
    
    Buffer buffer(cBuffer, cBufferSize);
    BMMessage msg = BMMessage(buffer);
    
    // Comput and check intital crc
    TS_ASSERT_EQUALS(msg.getCRC(), 0xe457);
    TS_ASSERT_EQUALS(msg.computeCRC(), 0xe457);
    TS_ASSERT_EQUALS(msg.isCRCValid(), true);
    
    // Write new payload, crc should fail
    msg.writePayload((uint8_t*)"abc", 4);
    TS_ASSERT_EQUALS(msg.computeCRC(), 0x19ba);
    TS_ASSERT_EQUALS(msg.isCRCValid(), false);
    
    // Write and validate new crc
    msg.setCRC();
    TS_ASSERT_EQUALS(msg.getCRC(), msg.computeCRC());
    TS_ASSERT_EQUALS(msg.getCRC(), 0x19ba);
    TS_ASSERT_EQUALS(msg.isCRCValid(), true);
    
    // Write dummy crc
    msg.setCRC(0x42);
    TS_ASSERT_EQUALS(msg.getCRC(), 0x42);
    TS_ASSERT_EQUALS(msg.computeCRC(), 0x19ba);
    TS_ASSERT_EQUALS(msg.isCRCValid(), false);
  }
  
  void testCRCWithoutPayload(void)
  {
    const uint16_t cBufferSize = 4;
    uint8_t cBuffer[cBufferSize] = {'a', 0, 0x25, 0x14};
    
    Buffer buffer(&cBuffer, cBufferSize);
    BMMessage msg = BMMessage(buffer);
    
    TS_ASSERT_EQUALS(msg.getCRC(), 0x2514)
    TS_ASSERT_EQUALS(msg.computeCRC(), 0x2514)
    TS_ASSERT_EQUALS(msg.isCRCValid(), true);
  }
  
  void testPayloadSmallBuffer(void)
  {
    const uint16_t cBufferSize = 4;
    uint8_t cBuffer[cBufferSize] = {'a', 0, 0xe4, 0x57};
    
    Buffer buffer(&cBuffer, cBufferSize);
    BMMessage msg = BMMessage(buffer);
   
    // Check if we can add an byte
    TS_ASSERT(msg.writePayload(cBuffer, 1) != 0);
    TS_ASSERT(msg.appendPayload('a') != 0);
  }
  
  void testPayloadInvalidSize(void)
  {
    const uint16_t cBufferSize = 4;
    uint8_t cBuffer[cBufferSize] = {'a', 1, 0xe4, 0x57};
    
    Buffer buffer(&cBuffer, cBufferSize);
    BMMessage msg = BMMessage(buffer);
    
    // Check if we can add an byte
    TS_ASSERT(msg.writePayload(cBuffer, 1) != 0);
    TS_ASSERT(msg.appendPayload('a') != 0);
  }
  
  void testPayload(void)
  {
    const uint16_t cBufferSize = 10;
    uint8_t cBuffer[cBufferSize] = {'a', 6, 'H', 'e', 'l', 'l', 'o', 0, 0xe4, 0x57};
    
    Buffer buffer(&cBuffer, cBufferSize);
    BMMessage msg = BMMessage(buffer);
    
    // Write payload
    msg.writePayload((uint8_t*)"Hi", 3);
    
    // Check the new payload
    TS_ASSERT_EQUALS(strcmp((char*)msg.getPayload(), "Hi"), 0);
    TS_ASSERT_EQUALS(msg.getPayloadSize(), 3);
    TS_ASSERT_EQUALS(msg.getSizeWithoutCRC(), 5);
    TS_ASSERT_EQUALS(msg.getSize(), 7);
    
    // The crc should now point to the "lo" form "Hello"
    TS_ASSERT_EQUALS(msg.getCRC(), 'l'<<8 | 'o');
    TS_ASSERT_EQUALS(msg.isCRCValid(), false);
    
    // An overlong payload should not be accepted!
    uint8_t string[] = "TooLong";
    TS_ASSERT(msg.writePayload(string, sizeof(string)) != 0);
    TS_ASSERT_EQUALS(strcmp((char*)msg.getPayload(), "Hi"), 0);
    
    // Delete String
    msg.writePayload(0,0);
    TS_ASSERT_EQUALS(msg.getPayloadSize(), 0);
    
    // Append a new string
    msg.appendPayload('a');
    msg.appendPayload('b');
    msg.appendPayload('c');
    msg.appendPayload('d');
    msg.appendPayload('e');
    msg.appendPayload('\0');
    TS_ASSERT(msg.appendPayload('a') != 0); // behind buffer
    TS_ASSERT_EQUALS(msg.getPayloadSize(), 6);
    TS_ASSERT_EQUALS(msg.getSizeWithoutCRC(), 8);
    TS_ASSERT_EQUALS(msg.getSize(), 10);
    TS_ASSERT_EQUALS(strcmp((char*)msg.getPayload(), "abcde"), 0);
    
    msg.setCRC();
    TS_ASSERT_EQUALS(strcmp((char*)msg.getPayload(), "abcde"), 0);
    TS_ASSERT_EQUALS(msg.getCRC(), msg.computeCRC());
    TS_ASSERT_EQUALS(msg.getCRC(), 0x6CED);
  }
  
  
  
};
