#include "include/task.h"
#include "kern_console.h"
typedef void (*sys_handler_t)(struct task *);


static void sys_print(struct task * task)
{
    puts("sys_print("); puthex(task->ctx.gpr[0]); puts(") @ ");
    puts(task->name);
    puts("\n");
    puts((char*)task->ctx.gpr[0]);
}

static void sys_yield(struct task * task)
{
    puts("sys_yield() @ ");
    puts(task->name);
    puts("\n");
    task_next();
}

static void sys_exit(struct task * task)
{

    puts("sys_exit() @ ");
    puts(task->name);
    puts("\n");
    task->state = TASK_DEAD;
    task_next(); // switch to next task
}

sys_handler_t syscall_handlers[] = {
    sys_print,
    sys_exit,
    sys_yield,
};


void do_syscall(int no, struct task *task)
{
    if (no > (sizeof(syscall_handlers) / sizeof(sys_handler_t))) {
        puts("Unknown syscall requested.\n");
        return;
    }

    syscall_handlers[no](task);
}
