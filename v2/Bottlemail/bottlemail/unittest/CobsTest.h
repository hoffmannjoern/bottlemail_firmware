//
//  CobsTest.h
//  bottlemail
//
//  Created by Jörn Hoffmann on 14.09.14.
//  Copyright (c) 2014 Jörn Hoffmann. All rights reserved.
//

#include "cxxtest/TestSuite.h"
#include <string.h>

extern "C"{
  #include "cobs.h"
}

class CobsTest : public CxxTest::TestSuite
{
  
public:
  void testEncode(void)
  {
    const int bufferSize = 10;
    char buffer[10] = {0x11, 0x22, 0x00, 0x33, 'x', 'x'};
    const char bufferCmp[10] = {0x3, 0x11, 0x22, 0x02, 0x33};

    // Check incorrect parameters
    int res = cobsEncode(buffer, 4, buffer, 4);
    TS_ASSERT_EQUALS(res, 0)
    
    res = cobsEncode(buffer, 4, buffer, 0);
    TS_ASSERT_EQUALS(res, 0)
    
    res = cobsEncode(buffer, 4, NULL, 5);
    TS_ASSERT_EQUALS(res, 0)
    
    res = cobsEncode(NULL, 4, buffer, 5);
    TS_ASSERT_EQUALS(res, 0)

    // Check correct parameters but no computation
    res = cobsEncode(buffer, 0, buffer, 5);
    TS_ASSERT_EQUALS(res, 1)
    TS_ASSERT_EQUALS(buffer[0], 0x11);

    // Check correct computation
    res = cobsEncode((unsigned char*)buffer, 4, buffer, 5);
    TS_ASSERT_EQUALS(res, 1)
    TS_ASSERT(memcmp(buffer, bufferCmp, 5) == 0);
    TS_ASSERT_EQUALS(buffer[5], 'x');
  }
  
  void testEncodeDecode(void)
  {
    const int bufferSize = 128;
    unsigned char *buffer = (unsigned char*) malloc(bufferSize);
    unsigned char *bufferCmp = (unsigned char*) malloc(bufferSize);
    
    for (unsigned i = 0; i < bufferSize; i++)
    {
      unsigned char c = i+1;
      if (i==254)
        c = 0;
      
      buffer[i] = c;
      bufferCmp[i] = c;
    }
    
    for (unsigned i = 0; i<bufferSize; i++)
      printf("\ni:%3d, 0x%.2x = %3d", i, buffer[i], buffer[i]);
    
    // Check correct computation
    int res = cobsEncode(buffer, bufferSize-2, buffer, bufferSize);
    TS_ASSERT_EQUALS(res, 1)
    
    // Result must be equal to the original elements
    TS_ASSERT(memcmp(buffer, bufferCmp, bufferSize) != 0);
    
    // Result must not contain a zero
    bool foundZero = false;
    for (unsigned i = 0; i<bufferSize; i++)
    {
      if (buffer[i] == 0) {
        foundZero = true;
        break;
      }
    }
    
    for (unsigned i = 0; i<bufferSize; i++)
      printf("\ni:%3d, 0x%.2x = %3d", i, buffer[i], buffer[i]);
    
    TS_ASSERT_EQUALS(foundZero, false);
    
    // Check decode result
    cobsDecode(buffer, bufferSize, buffer, bufferSize);
    TS_ASSERT(memcmp(buffer, bufferCmp, bufferSize-2) == 0);
    
    for (unsigned i = 0; i<bufferSize; i++)
      printf("\ni:%3d, 0x%.2x = %3d", i, buffer[i], buffer[i]);
    
  }
};

