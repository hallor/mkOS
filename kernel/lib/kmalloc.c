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

#include "kmalloc.h"
#include "page.h"
#include "printk.h"
#include "util.h"

// align do 16 bytes
#define ALIGN_UP(X) ((X + 0xF) & ~0xF)

struct memchunk {
    struct memchunk * next;
    void * data;
    unsigned offset; // pointer to first "free" data
    unsigned total_size; // total allocated size
};

static struct memchunk * mem = 0;

void kmalloc_init()
{
    info("Initializing kernel malloc pool...\n");
    mem = page_alloc(4);
    if (!mem)
        panic("failed to allocate kernel malloc pool.\n");
    mem->next = 0;
    mem->total_size = 4 << PAGE_SHIFT;
    mem->offset = 0;
    mem->data = mem;
    mem->offset += ALIGN_UP(sizeof(struct memchunk));
}

void * kmalloc(unsigned size)
{
    struct memchunk * m = mem, *mem_last = mem;
    // Try to find chunk
    size = ALIGN_UP(size);
    while (m) {
        if (m->total_size - m->offset > size) {
            m->offset += size;
            return m->data + m->offset - size;
        }
        mem_last = m;
        m = m->next;
    }
    dbg("did not found free mem chunk, allocating new one\n");
    m = page_alloc( ALIGN_PGUP(size + ALIGN_UP(sizeof(struct memchunk))) >> PAGE_SHIFT);
    if (!m) {
        err("out of memory\n");
        return 0;
    }
    mem_last->next = m;
    m->total_size = ALIGN_PGUP(size + ALIGN_UP(sizeof(struct memchunk)));
    m->offset = ALIGN_UP(sizeof(struct memchunk));
    m->data = m;
    m->next = 0;
    // Now allocate size elems
    m->offset += size;
    return m->data + m->offset - size;
}

void * kzalloc(unsigned size)
{
    void * p = kmalloc(size);
    if (p)
        memset(p, 0, size);
    return p;
}

void kfree(void * d)
{
    wrn("kfree not implemented.\n");
}
