#ifndef TASK_H
#define TASK_H
#include <stdint.h>
#include "config.h"

struct cpu_ctx {
    uint64_t gpr[30];
    uint64_t lr;
    uint64_t sp;
    uint64_t spsr;
    uint64_t pc; // ELR_el1
};

enum task_state {
    TASK_INVALID = 0,
    TASK_ACTIVE,
    TASK_DEAD,
};

struct task {
    struct cpu_ctx ctx; // must be first (for now)
    char name[CONFIG_MAX_FILE_NAME];
    unsigned tid;
    enum task_state state;
    void * vma_addr;
    unsigned vma_size;
};

extern struct task * current;

void task_create(void);
void task_next(void);
#endif // TASK_H
