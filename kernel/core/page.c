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
#include "bitops.h"
extern char _heap_start[];
// TODO: implement page merging

struct page // 4 bytes free here
{
    uint32_t start_page_no; // start address of this page
    struct page * next_page; // linked list of pages in the same zone
};

// each zone is for PAGE * 2^zone_no, so zones[0] == 4k pages, zones[1] == 8k pages, zones[2] == 16k pages...
#define MAX_ZONES 53
static struct page * zones[MAX_ZONES] = {0, }; // we can cover whole 64-bit address space if needed

// free page descriptors (for re-use)
static struct page * free = 0;

// pre-allocated pool of all pages (depending on size of ram)
static struct page * page_pool = 0;
// size of page pool (max number of pages)
static unsigned page_pool_size = 0;
// next unused page - for allocation when "free" list is empty
static unsigned page_pool_next_free = 0;

// start of mem pool (i.e. page->start_page is page offset of this address)
// Must be aligned to page
static void * mem_pool_start = 0;

// alocates free page to zone with order #order; it must me coherent
static void add_free_page_to_zone(unsigned page_offset, unsigned order)
{
    dbg("Add page to zone %d (%d pages) start: %d\n", order, 1 << order, page_offset);
    struct page * p = 0;
    if (free) {
        p = free;
        free = free->next_page;
    } else {
        if (page_pool_next_free < page_pool_size) {
            p = &page_pool[page_pool_next_free++];
        } else {
            panic("Page pool exhausted.\n");
        }
    }
    p->start_page_no = page_offset;
    p->next_page = zones[order];
    zones[order] = p;
}

static struct page* get_free_page_from_zone(unsigned order)
{
    struct page * r = 0;
    if (order >= MAX_ZONES)
        return 0;
    if (!zones[order])
        return 0;
    r = zones[order];
    zones[order] = r->next_page;
    return r;
}

// start is page # from mem_pool_start, size is size in page count
static void add_memory_to_pool(unsigned page_offset, unsigned page_cnt)
{
    dbg("Add memory to free pool: %d pages starting at page %d\n", page_cnt, page_offset);
    while (page_cnt) {
        unsigned biggest_page_order = (63 - clz(page_cnt)); // 2 ^ biggest_page_order pages
        add_free_page_to_zone(page_offset, biggest_page_order);
        page_offset += 1 << biggest_page_order; // 2^ biggest_page_order pages consumed
        page_cnt -= 1 << biggest_page_order; // remaining space
    }
}

// split bigger zones until zone 'order' has at least one page
// returns 0 on failure
static int split_zone(unsigned order)
{
    if (order >= MAX_ZONES)
        return 0;

    if (zones[order]) { // zone has already a page
        return 1;
    }

    if (split_zone(order + 1)) { // we must split bigger zone
        struct page * p = get_free_page_from_zone(order + 1);
        add_free_page_to_zone(p->start_page_no, order);
        add_free_page_to_zone(p->start_page_no + (1 << order), order);
        p->next_page = free; // put page back to pool
        free = p;
        return 1;
    }
    // oom
    return 0;
}

void dump_zones()
{
    int i, total = 0;
    dbg("Dumping zones\n");
    for (i=0; i<MAX_ZONES; ++i)
    {
        unsigned chunk_cnt = 0;
        if (zones[i]) {
            struct page * p = zones[i];
            while (p) {
                chunk_cnt++;
                p = p->next_page;
            }
            total += (1 << i) * chunk_cnt;
            dbg("zone %d (%d pages) contains %d chunks.\n", i, 1 << i, chunk_cnt);
        }
    }
    dbg("End of zone dump; total free pages: %d.\n", total);
}

// Get memory chunk from pool, size is multiply of pages (but doesn't have to be power-of-2)
static void * get_memory_from_pool(unsigned page_cnt)
{
    unsigned biggest_zone = (63 - clz(page_cnt));
    unsigned pages_mask = (1 << (63 - clz(page_cnt))) - 1;
    if (page_cnt & pages_mask)
        biggest_zone++;

    if (split_zone(biggest_zone)) {
        struct page * p = get_free_page_from_zone(biggest_zone);
        add_memory_to_pool(p->start_page_no + page_cnt, (1 << biggest_zone) - page_cnt);
        p->next_page = free;
        free = p;
        dbg("got pages, start page %d num pages %d\n", p->start_page_no, page_cnt);
        return mem_pool_start + (p->start_page_no << PAGE_SHIFT);
    }

    panic("No free (continuous) memory.\n");
    return 0;
}

int page_init(void)
{
    int ram_available = CONFIG_RAM_SIZE - ((void*)_heap_start - (void*)CONFIG_RAM_START);

    info("Initializing page allocator...\n");
    // Max number of 4k pages needed to cover whole ram (in fact, more than it will be needed as it includes
    // area that will be used to keep page pool
    page_pool = (void*)_heap_start;
    page_pool_size = ALIGN_PGUP(ram_available) >> PAGE_SHIFT;
    if (page_pool_size > 65536) {
        wrn("pages can't cover whole memory; capping at 65536 pages.\n");
        page_pool_size = 65536;
    }

    mem_pool_start = _heap_start + ALIGN_PGUP((sizeof(struct page) * page_pool_size));
    ram_available = ALIGN_PGDOWN(ram_available - (mem_pool_start - (void*)_heap_start));
    add_memory_to_pool(0, ram_available >> PAGE_SHIFT);

    info("Heap start: %#llx, heap size: %lldK, used %lldK for metadata\n",
         mem_pool_start, ram_available >> 10, (mem_pool_start - (void*)_heap_start) >> 10);
    dump_zones();

    return 0;
}

void *page_alloc(int num_pages)
{
    return get_memory_from_pool(num_pages);
}

void page_free(void *paddr, int num_pages)
{
    dbg("page_free start page: %d num pages: %d\n", (uintptr_t) (paddr - mem_pool_start) >> PAGE_SHIFT, num_pages);
    add_memory_to_pool((uintptr_t)(paddr - mem_pool_start) >> PAGE_SHIFT, num_pages);
}
