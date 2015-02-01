#ifndef PRINTK_H
#define PRINTK_H
int printk(const char *format, ...);
void puts(const char * s);

#define __sys_msg(CO, CL, X,...) printk(CO"[$][%s] %s:%d "X"\e[37;40;0m", CL, __THIS_FILE__, __LINE__,##__VA_ARGS__)

#define dbg(...) __sys_msg("\e[2;37m","DEBUG", __VA_ARGS__)
#define info(...) __sys_msg("\e[32m","INFO", __VA_ARGS__)
#define wrn(...) __sys_msg("\e[34m","WARN", __VA_ARGS__)
#define err(...) __sys_msg("\e[31m","ERROR", __VA_ARGS__)

#define panic(X,...) do { \
    puts("\n\n\e[31;40;1m[PANIC]"__FILE__":"); puts(__FUNCTION__ ); \
    printk("():%d "X"\e[37;40;0m", __LINE__, ##__VA_ARGS__); \
    while (1) { asm("wfi"); } \
    } while (0)

#endif // PRINTK_H
