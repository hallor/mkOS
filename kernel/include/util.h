#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <stddef.h>

void memcpy(void * dest, void * src, unsigned cnt);
int memcmp(void * d1, void * d2, unsigned cnt);
void memset(void * dest, int c, unsigned n);
int strncmp(const char *s1, const char *s2, unsigned n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
uint32_t ntohl(uint32_t d);
#endif // UTIL_H
