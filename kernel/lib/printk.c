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
