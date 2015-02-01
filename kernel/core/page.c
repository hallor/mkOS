/*
  Copyright (C) 2015 Mateusz Kulikowski <mateusz.kulikowski@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include "config.h"
#include "page.h"
#include "util.h"
#include "printk.h"
extern char _heap_start[];

static char *page_allocation_bitmap = 0;
static unsigned page_allocation_bitmap_size;

int page_init(void)
{
    int free_ram = CONFIG_RAM_SIZE - ((void*)_heap_start - (void*)CONFIG_RAM_START);

    info("Initializing page allocator...\n");
    dbg("Heap start: %#llx size: %d\n", _heap_start, free_ram);
    page_allocation_bitmap_size = free_ram >> PAGE_SHIFT; //# of entries for page bitmap
    page_allocation_bitmap = _heap_start;
    memset(page_allocation_bitmap, 0, page_allocation_bitmap_size);

    int no_used = ALIGN_PGUP(page_allocation_bitmap_size) >> PAGE_SHIFT;
    dbg("page bitmap size: %d / %d pages\n", page_allocation_bitmap_size, no_used);
    page_allocation_bitmap = page_alloc(no_used);
    return 0;
}

void *page_alloc(int num_pages)
{
    int i, num_free=0;

    if (num_pages <= 0) {
        wrn("Failed to allocate page.\n");
        return 0;
    }

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
    wrn("Failed to allocate page.\n");
    return 0;
}
