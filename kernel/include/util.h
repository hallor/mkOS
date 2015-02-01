#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>

#define NULL 0
void memcpy(void * dest, void * src, unsigned cnt);
void memset(void * dest, int c, unsigned n);
uint32_t ntohl(uint32_t d);
#endif // UTIL_H
