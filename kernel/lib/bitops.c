#include "bitops.h"


int clz(uint64_t v)
{
    uint64_t result;
    asm("clz %0, %1" : "=r"(result) : "r"(v));
    return result;
}
