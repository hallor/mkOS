#include <stdint.h>
#include "config.h"
#include "page.h"
#include "kern_console.h"
#include "util.h"
extern char _heap_start[];

static char *page_allocation_bitmap = 0;
static unsigned page_allocation_bitmap_size;


int page_init(void)
{
    int free_ram = CONFIG_RAM_SIZE - ((void*)_heap_start - (void*)CONFIG_RAM_START);
    putreg("Kernel heap start", _heap_start);
    putreg("Kernel heap size", free_ram);

    page_allocation_bitmap_size = free_ram >> PAGE_SHIFT; //# of entries for page bitmap
    putreg("Bitmap size", page_allocation_bitmap_size);
    page_allocation_bitmap = _heap_start;
    memset(page_allocation_bitmap, 0, page_allocation_bitmap_size);

    int no_used = ALIGN_PGUP(page_allocation_bitmap_size) >> PAGE_SHIFT;
    putreg("Pages used for bitmap", no_used);
    page_allocation_bitmap = page_alloc(no_used);
    return 0;
}

void *page_alloc(int num_pages)
{
    int i, num_free=0;

    if (num_pages <= 0)
        return 0;

    for (i=0; i<page_allocation_bitmap_size; ++i) {
        if (page_allocation_bitmap[i] == 0) {
            num_free++;
        } else {
            num_free = 0;
        }

        if (num_free == num_pages) {
            int j;
            for (j=0; j < num_free; ++j)
                page_allocation_bitmap[i + 1 - num_free + j] = 1;
            return _heap_start + ((i + 1 - num_free) << PAGE_SHIFT);
        }
    }
    return 0;
}
