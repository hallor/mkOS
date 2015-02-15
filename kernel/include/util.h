#ifndef UTIL_H
#define UTIL_H
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
#include <stddef.h>

void memcpy(void * dest, void * src, unsigned cnt);
int memcmp(void * d1, void * d2, unsigned cnt);
void memset(void * dest, int c, unsigned n);
int strncmp(const char *s1, const char *s2, unsigned n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);
uint32_t ntohl(uint32_t d);

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#endif // UTIL_H
