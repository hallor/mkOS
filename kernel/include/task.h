#ifndef TASK_H
#define TASK_H
#include <stdint.h>

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
    char name[16];
    enum task_state state;
};

extern struct task * current;

void task_create(void);
void task_next(void);
#endif // TASK_H
