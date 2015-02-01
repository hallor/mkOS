void memcpy(void * dest, void * src, unsigned cnt)
{
    while (cnt--) {
        *((char*)dest) = *((char*)src);
        dest++;
        src++;
    }
}
