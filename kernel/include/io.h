#ifndef IO_H
#define IO_H
#include <stdint.h>

#define ioread8(addr)  (*)( uint8_t*)((uintptr_t)addr)))
#define ioread16(addr) (*((uint16_t*)((uintptr_t)addr)))
#define ioread32(addr) (*((uint32_t*)((uintptr_t)addr)))
#define ioread64(addr) (*((uint64_t*)((uintptr_t)addr)))

#define iowrite8(addr, val)  (*(( uint8_t*)((uintptr_t)addr)) = (val))
#define iowrite16(addr, val) (*((uint16_t*)((uintptr_t)addr)) = (val))
#define iowrite32(addr, val) (*((uint32_t*)((uintptr_t)addr)) = (val))
#define iowrite64(addr, val) (*((uint64_t*)((uintptr_t)addr)) = (val))

#endif // IO_H
