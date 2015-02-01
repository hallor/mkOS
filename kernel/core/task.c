#include "include/task.h"
#include "include/page.h"
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

//char * ram_dest = (void*)0x80100000;
//unsigned ram_len = 1024;
//memcpy(ram_dest, flash_base, ram_len);

void task_create()
{
    int i;
    char name[16]="Task_#";
    volatile char * code_base = (void*)0x08000000;
    puts("Creating tasks...\n");
    for (i=0; i<MAX_TASKS; ++i) {
        struct cpu_ctx * ctx = &tasks[i].ctx;
        name[6] = '0' + i;
        name[7] = 0;
        memcpy(tasks[i].name, name, 7);
        ctx->gpr[0] = i; // pass task #
        ctx->lr = 0x0;
        ctx->sp = page_alloc(1);
        ctx->spsr = 0x3C0; // User, AA64
        tasks[i].state = TASK_ACTIVE;
        tasks[i].vma_addr = page_alloc(1);
        tasks[i].vma_size = PAGE_SIZE;
        memcpy(tasks[i].vma_addr, code_base, PAGE_SIZE); // copy task code
        ctx->pc = tasks[i].vma_addr + 0x28;//0x80100028; // Entry point
        puts("Allocated task @ "); puthex(tasks[i].vma_addr); puts("\n");
    }
}
