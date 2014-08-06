#ifndef STDIO_H
#define STDIO_H
#include <stdint.h>
void putc(int c);
int getc(void);
void puts(const char * s);
void gets(char * s, int n);
void puthex(uint64_t n);
#endif // STDIO_H
