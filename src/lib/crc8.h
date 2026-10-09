#ifndef CRC8_H
#define CRC8_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


#define CRC8_INIT	0xFF
#define CRC8_OK		0x00


extern uint8_t crc8_table[256];

#define CRC8_BYTE(crc, b)	(crc8_table[(b) ^ (crc)])

uint8_t CRC8(uint8_t crc, const uint8_t *data, uint16_t len);


#ifdef __cplusplus
}
#endif


#endif
