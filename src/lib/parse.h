#ifndef LIB_PARSE_H
#define LIB_PARSE_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


int32_t parse_int(const char *s);
int64_t parse_int64(const char *s);
uint32_t parse_uint(const char *s);
uint64_t parse_uint64(const char *s);
float parse_float(const char *s);
uint8_t parse_ip(const char *s, uint32_t *ip);


#ifdef __cplusplus
}
#endif


#endif
