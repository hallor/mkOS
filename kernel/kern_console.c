#include <stdint.h>
#include "uart.h"
#include "kern_console.h"

void putc(int c)
{
	uart_put(c);
}

int getc(void)
{
	int c = uart_get();
	putc(c);
	return c;
}

void puts(const char * s)
{
	if (!s)
		return;
	while (*s) {
		putc(*s++);
	}
}

void gets(char * s, int n)
{
	if (!s)
		return;

	n--;
	s[n]=0;
	while (n--) {
		*s= getc();
		if (*s == '\r') {
			*s = 0;
			return;
		}
		s++;
	}
}

void puthex(uint64_t n)
{
	int zeros = 1;
	char c;
	int i;
	puts("0x");
	for (i=15; i>=0; i--) {
		c = (n >> (4 * i)) & 0xF;
		if (c != 0)
			zeros = 0;
		if (zeros)
			continue;
		if (c < 10)
			putc('0' + c);
		else
			putc('A' + (c - 10));
	}
	if (zeros)
		putc('0');
}

void putreg(const char * name, uint64_t val)
{
	puts(name);
	puts(": ");
	puthex(val);
	putc('\n');
}
