#ifndef PRINTK_H
#define PRINTK_H
int printk(const char *format, ...);

#define __sys_msg(CL, X,...) printk("[%s] %s:%d "X, CL, __THIS_FILE__, __LINE__,##__VA_ARGS__)

#define dbg(...) __sys_msg("DEBUG", __VA_ARGS__)
#define wrn(...) __sys_msg("WARN", __VA_ARGS__)
#define info(...) __sys_msg("INFO", __VA_ARGS__)
#define ERR(...) __sys_msg("ERROR", __VA_ARGS__)

#define panic(X,...) do { \
    puts("\n\n[PANIC]"__FILE__":"); puts(__FUNCTION__ ); \
    printk("():%d "X, __LINE__, ##__VA_ARGS__); \
    while (1) { asm("wfe"); } \
    } while (0)

#endif // PRINTK_H
