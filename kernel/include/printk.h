#ifndef PRINTK_H
#define PRINTK_H
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
#include "config.h"

int printk(const char *format, ...);
void puts(const char * s);

#define __sys_msg(CO, CL, X,...) printk(CO"[$][%5s] %5s:%d "X"\e[37;40;0m", CL, __THIS_FILE__, __LINE__,##__VA_ARGS__)

#if (PRINTK_LEVEL <= PRINTK_LEVEL_DBG)
#define dbg(...) __sys_msg("\e[2;37m","DEBUG", __VA_ARGS__)
#else
#define dbg(...)
#endif

#if (PRINTK_LEVEL <= PRINTK_LEVEL_INFO)
#define info(...) __sys_msg("\e[32m","INFO", __VA_ARGS__)
#else
#define info(...)
#endif

#if (PRINTK_LEVEL <= PRINTK_LEVEL_WRN)
#define wrn(...) __sys_msg("\e[34m","WARN", __VA_ARGS__)
#else
#define wrn(...)
#endif

#if (PRINTK_LEVEL <= PRINTK_LEVEL_ERR)
#define err(...) __sys_msg("\e[31m","ERROR", __VA_ARGS__)
#else
#define err(...)
#endif

#define panic(X,...) do { \
    puts("\n\n\e[31;40;1m[PANIC]"__FILE__":"); puts(__FUNCTION__ ); \
    printk("():%d "X"\e[37;40;0m", __LINE__, ##__VA_ARGS__); \
    while (1) { asm("wfi"); } \
    } while (0)

#endif // PRINTK_H
