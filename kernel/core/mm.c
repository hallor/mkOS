#include <stdint.h>
#include "mm.h"
#include "printk.h"
#include "page.h"

typedef uint64_t entry;
enum granule {
    PG_4K,
    PG_16K,
    PG_64K
};

static entry make_invalid()
{
    return 0x0;
}

// addr == 64-bit address, not shifted
static entry make_block(uint64_t addr, unsigned uba, unsigned lba, enum granule g, unsigned level)
{
    entry r = 0x1;
    r |= (((entry)uba & 0xFFF) << 52 );
    r |= (((entry)lba & 0x3FF) << 2);
    switch(g) {
    case PG_4K:
        if (level == 1)
            r |= (entry)addr & ((entry)0x3FFFF << 30);
        else if (level == 2)
            r |= (entry)addr & ((entry)0x7ffffff << 21);
        else
            panic("Invalid entry requested.\n");
        break;
    case PG_16K:
        if (level == 2)
            r |= (entry)addr & ((entry)0x7fffff << 25);
        else
            panic("Invalid entry requested.\n");
        break;
    case PG_64K:
        if (level == 2)
            r |= (entry)addr & ((entry)0x7ffff << 29);
        else
            panic("Invalid entry requested.\n");
        break;
    }
    return r;
}

enum table_flag {
    TABLE_NS = 0x10,
    TABLE_AP3 = 0xC,
    TABLE_AP2 = 0x8,
    TABLE_AP1 = 0x4,
    TABLE_XN = 0x2,
    TABLE_PXN = 0x1
};

// addr == 64-bit address, not shifted
static entry make_table(uint64_t next_table_addr, int flags, enum granule g)
{
    entry r = 0x3;
    r |= ((entry)flags & 0x1F) << 59;
    switch (g) {
    case PG_4K:
        r |= (entry)next_table_addr & ((entry)0xFFFFFFFFF << 12);
        break;
    case PG_16K:
        r |= (entry)next_table_addr & ((entry)0x3FFFFFFFF << 14);
        break;
    case PG_64K:
        r |= (entry)next_table_addr & ((entry) 0xFFFFFFFF << 16);
        break;
    }
    return r;
}
// TODO: l3 descriptors

void mmu_init(void)
{
    info("Initializing mmu...\n");
}
