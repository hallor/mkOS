void memset(void * dest, int c, unsigned n)
{
    while (n--) {
        *(char*)dest = c;
        dest++;
    }
}
