#ifndef LIB_CRC32_H
#define LIB_CRC32_H


#include <inttypes.h>


#ifdef __cplusplus
extern "C" {
#endif


#define CRC32_INIT	0x00000000
#define CRC32_OK	0x2144DF1C


uint32_t CRC32(uint32_t crc, const uint8_t *data, uint32_t size);


#ifdef __cplusplus
}
#endif


#endif
