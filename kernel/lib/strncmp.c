#include "util.h"

int strncmp(const char *s1, const char *s2, unsigned n)
{
    int a;
    while (n--)
    {
        a = *s1 - *s2;
        if (a)
            return a;
        if (*s1 == 0)
            return 0;
        s1++;
        s2++;
    }
    return 0;
}
