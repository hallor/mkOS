#include "util.h"

int memcmp(void * d1, void * d2, unsigned cnt)
{
    char *a = d1, *b = d2;
    int r;
    while (cnt--) {
        r = *a++ - *b++;
        if (r)
            return r;
    }
    return 0;
}

