#include "kmalloc.h"
#include "page.h"
#include "printk.h"

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

void kfree(void * d)
{
    wrn("kfree not implemented.\n");
}
