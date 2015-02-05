#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL0(N) \
    inline uint64_t syscall##N(void) { \
        uint64_t rv; \
        asm("svc "#N"\n\t": "=r"(rv) : ); \
        return rv; \
    }

#define SYSCALL1(N) \
    uint64_t syscall##N(uint64_t a1) { \
        uint64_t rv; \
        asm("svc "#N"\n\t": "=r"(rv) : "r"(a1)); \
        return rv; \
    }

#define SYSCALL2(N) \
    inline uint64_t syscall##N(uint64_t a1, uint64_t a2) { \
        uint64_t rv; \
        asm("svc "#N"\n\t": "=r"(rv) : "r"(a1), "r"(a2)); \
        return rv; \
    }


#endif // SYSCALL_H
