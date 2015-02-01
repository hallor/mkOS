#ifndef KERN_CONSOLE_H
#define KERN_CONSOLE_H
#include <stdint.h>

void putc(int c);
int getc(void);
void puts(const char * s);
void gets(char * s, int n);
void puthex(uint64_t n);

void putreg(const char *name, uint64_t val);
#endif // KERN_CONSOLE_H
