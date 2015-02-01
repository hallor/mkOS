#include <stdint.h>

uint32_t ntohl(uint32_t d)
{
    return  ((d & 0x000000ffUL) << 24) |
            ((d & 0x0000ff00UL) <<  8) |
            ((d & 0x00ff0000UL) >>  8) |
            ((d & 0xff000000UL) >> 24);
}
