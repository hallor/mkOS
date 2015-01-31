#include "include/task.h"
#include "kern_console.h"

#define MAX_TASKS 2
struct task tasks[MAX_TASKS] = { {0, }, };

struct task *current = &tasks[0];

void task_next()
{
    if (current == &tasks[0])
        current = &tasks[1];
    else
        current = &tasks[0];
}


// Creates basic tasks
//ldr x0, =_user_stack_top // setup EL0 SP
void task_create()
{
    int i;
    puts("Creating tasks...\n");
    for (i=0; i<MAX_TASKS; ++i) {
        struct cpu_ctx * ctx = &tasks[i].ctx;
        ctx->lr = 0x0;
        ctx->sp = 0x0;
        ctx->spsr = 0x3C0; // User, AA64
        ctx->pc = 0x80100028; // Entry point
    }
}
