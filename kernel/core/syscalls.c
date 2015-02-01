#include "task.h"
#include "printk.h"
typedef void (*sys_handler_t)(struct task *);


static void sys_print(struct task * task)
{
    dbg("(%s:%d) sys_print(%#llx)\n", task->name, task->tid, task->ctx.gpr[0]);
    puts((char*)task->ctx.gpr[0]);
}

static void sys_yield(struct task * task)
{
    dbg("(%s:%d) sys_yield()\n", task->name, task->tid);
    task_next();
}

static void sys_exit(struct task * task)
{
    dbg("(%s:%d) sys_exit()\n", task->name, task->tid);
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
        err("Unknown syscall requested: %d.\n", no);
        return;
    }

    syscall_handlers[no](task);
}
