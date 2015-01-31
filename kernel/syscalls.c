#include "include/task.h"
#include "kern_console.h"
typedef void (*sys_handler_t)(struct task *);


static void sys_print(struct task * task)
{
    puts("sys_print()\n");
}

static void sys_exit(struct task * task)
{
    puts("sys_exit()\n");
    while(1) {}
}

sys_handler_t syscall_handlers[] = {
    sys_print,
    sys_exit,
};


void do_syscall(int no, struct task *task)
{
    if (no > (sizeof(syscall_handlers) / sizeof(sys_handler_t))) {
        puts("Unknown syscall requested.\n");
        return;
    }

    syscall_handlers[no](task);
}
