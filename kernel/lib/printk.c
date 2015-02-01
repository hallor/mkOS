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

#include <stdarg.h>
#include "config.h"
#include "printf.h"
#include "kern_console.h"
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);

static char printf_buf[CONFIG_PRINTK_BUFFER_LEN] = {0, };

int printk(const char *format, ...)
{
    va_list ap;
    int len;

    printf_buf[0] = 0;
    printf_buf[CONFIG_PRINTK_BUFFER_LEN - 1] = 0;

    va_start(ap, format);
    len =  vsnprintf(printf_buf, CONFIG_PRINTK_BUFFER_LEN - 1, format, ap);
    va_end(ap);

    puts(printf_buf);

    return len;
}
