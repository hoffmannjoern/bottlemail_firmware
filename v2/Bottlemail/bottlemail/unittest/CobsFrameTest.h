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
    const int cBufferSize    = 10;
    uint8_t cBuffer[10]      = {0x65, 0x66, 0x00, 0x68};
    uint8_t cBufferEncoded[] = {0x03, 0x65, 0x66, 0x02, 0x68};
    
    Buffer buffer(cBuffer, cBufferSize);
    CobsFrame frame(buffer, 4, false);
    
    // Check creation
    TS_ASSERT_EQUALS(frame.getLength(), 4);
    TS_ASSERT_EQUALS(frame.isEncoded(), false);
    
    printf("frame: %s\n", cBuffer);
    
    // Check encode
    bool ret = frame.encode();
    TS_ASSERT_EQUALS(ret, true);
    TS_ASSERT_EQUALS(frame.getLength(), 5);
    TS_ASSERT_EQUALS(frame.isEncoded(), true);
    
    
    ret = frame.decode();
    TS_ASSERT_EQUALS(ret, true);
    
    printf("frame %s\n", cBuffer);
  }

};
