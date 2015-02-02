#ifndef CPU_H
#define CPU_H

struct cpu_ctx {
    uint64_t gpr[30];
    uint64_t lr;
    uint64_t sp;
    uint64_t spsr;
    uint64_t pc; // ELR_el1
};

#endif // CPU_H
