/*
 * endian.h
 *
 * Created: 25.09.2014 13:13:11
 *  Author: jhoffmann
 */ 


#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <stdint.h>

#define msb(x) ((uint8_t) ((x) >> ((sizeof(x)-1) * 8)))
#define lsb(x) ((uint8_t) ((x) & 0xff))

#ifndef HTONS
  #ifdef __BYTE_ORDER__
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      #define HTONS(x) ((uint16_t) (((uint16_t) (x) << 8) | ((uint16_t) (x) >> 8)))
      #define HTONL(x) ((uint32_t) (((uint32_t) HTONS(x) << 16) | HTONS((uint32_t) (x) >> 16)))

    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      #define HTONS(x) ((uint16_t) (x))
      #define HTONL(x) ((uint32_t) (x))

    #else
      #error Byte order not supported!
    #endif

  #else
    #error Byte order not defined!
  #endif
   
  #define NTOHS(x) HTONS(x)
  #define NTOHL(x) HTONL(x)
#endif

#endif /* __ENDIAN_H__ */
