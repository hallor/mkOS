#include "include/page.h"
#include "kern_console.h"
//static void * ram_start = (void*)0x80100000;
static int free_ram_size = 0x100000;
static void * ram_next = (void*)0x80100000;

void *page_alloc(int num_pages)
{
    int alloc_size = num_pages * PAGE_SIZE;
    if (alloc_size > free_ram_size) {
        puts("alloc_pages: No free memory\n");
        return 0;
    }
    free_ram_size -= alloc_size;
    ram_next += alloc_size;
    return ram_next - alloc_size;
}
