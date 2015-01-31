#include "include/task.h"
#include "kern_console.h"

#define MAX_TASKS 2
struct task tasks[MAX_TASKS] = { {0, }, };

struct task *current = &tasks[0];
static int current_no = 0;

void task_next()
{
    int i;
    for (i=1; i<MAX_TASKS; ++i) {
        if (tasks[(i + current_no) % MAX_TASKS].state == TASK_ACTIVE) {
            current_no = (i + current_no) % MAX_TASKS;
            current = &tasks[current_no];
        }
    }

    if (current->state != TASK_ACTIVE) {
        puts("Kernel panic - no more tasks are active.\n");
        while (1) {};
    }
    puts("task_next()->");
    puts(current->name);
    puts("\n");
}

void memcpy(void * dest, void * src, unsigned cnt);
// Creates basic tasks
//ldr x0, =_user_stack_top // setup EL0 SP
void task_create()
{
    int i;
    char name[16]="Task_#";
    puts("Creating tasks...\n");
    for (i=0; i<MAX_TASKS; ++i) {
        struct cpu_ctx * ctx = &tasks[i].ctx;
        name[6] = '0' + i;
        name[7] = 0;
        memcpy(tasks[i].name, name, 7);
        ctx->lr = 0x0;
        ctx->sp = 0x0;
        ctx->spsr = 0x3C0; // User, AA64
        ctx->pc = 0x80100028; // Entry point
        tasks[i].state = TASK_ACTIVE;
    }
}
