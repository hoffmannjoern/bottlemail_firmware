// MyTestSuite1.h

#include "cxxtest/TestSuite.h"
#include "CobsFrame.h"
#include "Buffer.hpp"
#include <string.h>

class CobsFrameTest : public CxxTest::TestSuite
{
  
public:
  void testCreate(void)
  {
    const int cBufferSize = 10;
    uint8_t cBuffer[10] = {0x11, 0x22, 0x00, 0x33};
    
    Buffer buffer(cBuffer, cBufferSize);
    CobsFrame frame(buffer, 4, false);
    
    // Check creation
    TS_ASSERT_EQUALS(frame.getLength(), 4);
    TS_ASSERT_EQUALS(frame.isEncoded(), false);
  }
  
  void testEncodeDecode(void)
  {
    const int payloadLength = 6;
    uint8_t cBuffer[payloadLength+1] = {0x61, 0x62, 0x63, 0x0, 0x64, 0x65};
    const uint8_t cBufferDecoded[]   = {0x61, 0x62, 0x63, 0x0, 0x64, 0x65};
    const uint8_t cBufferEncoded[]   = {0x04, 0x61, 0x62, 0x63, 0x03, 0x64, 0x65};
    
    Buffer buffer(cBuffer, sizeof(cBuffer));
    CobsFrame frame(buffer, payloadLength, false);
    
    // Check creation
    TS_ASSERT_EQUALS(frame.getLength(), payloadLength);
    TS_ASSERT_EQUALS(frame.isEncoded(), false);
    
    // Check encode
    bool ret = frame.encode();
    TS_ASSERT_EQUALS(ret, true);
    TS_ASSERT_EQUALS(frame.getLength(), payloadLength+1);
    TS_ASSERT_EQUALS(frame.isEncoded(), true);
    TS_ASSERT_EQUALS(memcmp(cBuffer, cBufferEncoded, sizeof(cBufferEncoded)), 0);
    
    // Check decode
    ret = frame.decode();
    TS_ASSERT_EQUALS(ret, true);
    TS_ASSERT_EQUALS(frame.getLength(), payloadLength);
    TS_ASSERT_EQUALS(frame.isEncoded(), false);
    TS_ASSERT_EQUALS(memcmp(cBuffer, cBufferDecoded, sizeof(cBufferDecoded)), 0);
  }

};
