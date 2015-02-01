/*
  Copyright (C) 2015 Mateusz Kulikowski <mateusz.kulikowski@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include "stdio.h"


inline int syscall0(int no)
{
	register int rv;
	asm("svc 0\n\t" : "=r"(rv) :"r"(no));
	return rv;
}

inline int syscall1(int no, int arg1)
{
	register int rv;
	asm("svc 0\n\t" : "=r"(rv) :"r"(no),"r"(arg1));
	return rv;
}

inline int syscall2(int no, int arg1, int arg2)
{
	register int rv;
	asm("svc 0\n\t" : "=r"(rv) :"r"(no),"r"(arg1),"r"(arg2));
	return rv;
}

void putc(int c)
{
	syscall1(0, c);
}

int getc(void)
{
	return syscall0(1);
}

void puts(const char * s)
{
	while (*s)
		putc(*s++);
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

