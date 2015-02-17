#include <stdint.h>
#include "mm.h"
#include "printk.h"
#include "page.h"
#include "arch_helpers.h"
#include "util.h"

typedef uint64_t entry;
enum granule {
    PG_4K,
    PG_16K,
    PG_64K
};

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

entry l0table_ttbr0[512] __attribute__((aligned(4096 * 8))); // lower, use 4k pages, 9bit resolved in lookup
entry l1table_ttbr0[512] __attribute__((aligned(4096 * 8))); // lower

entry l0table_ttbr1[512] __attribute__((aligned(4096 * 8))); // lower, use 4k pages, 9bit resolved in lookup
entry l1table_ttbr1[512] __attribute__((aligned(4096 * 8))); // lower

// we will map for now only 32-bit address space, so l0table has only 1 valid entry

static void dump_table(entry *table, unsigned no_entries, uint64_t va_offset, int level)
{
    int i;
    printk("Dumping table level %d at %p\n", level, table);
    for (i=0; i< no_entries; ++i)
    {
        entry ent = table[i];
        if (ent) {
//            printk("%016llx\n", ent);
            if ( (ent & 0x3) == 1) { // block
                printk("Entry %d - Block VA: %#18llx PA: %#18llx ", i,
                       va_offset + ((uint64_t)i << 30),
                       ent & ((entry)0x3FFFF << 30));
                printk("UXN: %x PXN: %x Cont: %x nG: %x, AF: %x SH:%x AP:%x NS: %x AttrIndx: %x\n",
                       ent >> 54 & 0x1, ent >> 53 & 0x1, ent >> 52 & 0x1,
                       ent >> 11 & 0x1, ent >> 10 & 0x1, ent >> 8 & 0x3,
                       ent >> 6 & 0x3, ent >> 5 & 0x1, ent >> 2 & 0x7);
            } else if ( (ent & 0x3) == 3) { // table
                printk("Entry %d - Table NS: %x AP: %x UXN: %x PXN: %x\n",
                       i, ent >> 63, ent >> 61 & 0x3, ent >> 60 & 0x1, ent >> 59 & 0x1);
                dump_table(ent & ((entry)0xFFFFFFFFF << 12), 512, va_offset + ((uint64_t)i << (39 - level * 9)), level + 1); // works for level 0 and 1 ;)
            } else {
                printk("Unknown table entry: %016llx\n", ent);
            }
        }
    }
}

static void dump_mmu(void)
{
    printk("MMU dump.\n");
    printk("MMU is %s. TTBR0=0x%016llx TTBR1=0x%016llx TCR=0x%016llx\n", read_sctlr_el1() & 0x1 ?
               "enabled" : "disabled", read_ttbr0_el1(), read_ttbr1_el1(), read_tcr_el1());

    uint64_t reg = read_tcr_el1();
    printk("TCR TBI1: %x TBI0: %x, AS: %x IPS: %x TG1: %x SH1: %x ORGN1: %x IRGN1: %x"
           "EPD1: %x A1: %x T1SZ: %x TG0: %x SH0: %x ORGN0: %x IRGN0: %x EPD0: %x T0SZ: %x\n",
           reg >> 38 & 0x1, reg >> 37 & 0x1, reg >> 36 & 0x1,
           reg >> 32 & 0x7, reg >> 30 & 0x3, reg >> 28 & 0x3, reg >> 26 & 0x3,
           reg >> 24 & 0x3, reg >> 23 & 0x1, reg >> 22 & 0x1, reg >> 16 & 0x3f,
           reg >> 14 & 0x3, reg >> 12 & 0x3, reg >> 10 & 0x3, reg >> 8 & 0x3,
           reg >> 7 & 0x1, reg & 0x3f);
    printk("Lower table:\n");
    dump_table(read_ttbr0_el1(), 512, 0, 0);
    printk("Upper table:\n");
    dump_table(read_ttbr1_el1(), 512, 0xFFFF000000000000LL, 0);
}

void mmu_init(void)
{
    int i;
    info("Initializing mmu...\n");
    memset(l0table_ttbr0, 0, sizeof(l0table_ttbr0));
    memset(l1table_ttbr0, 0, sizeof(l1table_ttbr0));
    memset(l0table_ttbr1, 0, sizeof(l0table_ttbr1));
    memset(l1table_ttbr1, 0, sizeof(l1table_ttbr1));

    // "userspace" mapping + mirror at high addresses
    unsigned table_desc_flags = // page 1804
            0 | // PXN
            0 << 1 | // XN
            0 << 2 | // AP - 01 = r/w access allowed for everyone, 00 - rw priv, r user
            0 << 4; // NS - Secure PA space

    for (i=0; i<1; ++i) {
    l0table_ttbr0[i] = make_table((uintptr_t)l1table_ttbr0, table_desc_flags, PG_4K); // TODO: flags
    l0table_ttbr1[i] = make_table((uintptr_t)l1table_ttbr1, table_desc_flags, PG_4K); // TODO: flags
    }
    // just 4 entries - 1gb each to cover whole 32-bit address space (for now)
    for (i=0; i<4; ++i) {
        // page 1796
        unsigned block_flags_upper =
                0 << 2 | // XN
                0 << 1 | // PXN
                0 << 0 | // conti - maybe enabled - cache of entries in TLB
                0;
        unsigned block_flags_lower =
                0 << 9 | // nG - translation is global
                1 << 8 | // AF - Access Flag - 1 - mark page as accessed (don't generate PG fault)
                0 << 6 | // SH[1:0] - non shareable
                1 << 4 | // AP[2:1] - rw priv, r user
                0 << 3 | // NS - secure
                0 << 0 | // AttrIndx - can be 0 as MAIR is set to 0 for all attributes
                0;


        l1table_ttbr0[i] = make_block((uint64_t)i << 30,
                                      block_flags_upper,
                                      block_flags_lower,
                                      PG_4K, 1);
        l1table_ttbr1[i] = make_block((uint64_t)i << 30,
                                      block_flags_upper,
                                      block_flags_lower,
                                      PG_4K, 1);
    }

    write_ttbr0_el1((uintptr_t)l0table_ttbr0);
    write_ttbr1_el1((uintptr_t)l0table_ttbr1);
    write_tcr_el1(
                0LL |
                0LL  << 38 | // TBI1 -> top byte used for address calculation
                0LL  << 37 | // TBI0 -> top byte used for address calculation
                0LL  << 36 | // AS - ASID size
                0LL  << 32 | // IPS - 32bit IPA
                2LL  << 30 | // TG1, granularity - 4kb
                0  << 28 | // SH1 - shareability
                0  << 26 | // ORGN1- outer cacheability
                0  << 24  | // IRGN1- inner cacheability
                0  << 23 | //EPD1
                0  << 22 | // A1 - ttbr0 defines asid
                24 << 16 | // T1SZ
                0  << 14 | // TG0, granularity - 4kb
                0  << 12 | // SH0 - shareability
                0  << 10 | // ORGN0 - outer cacheability
                0  << 8  | // IRGN0- inner cacheability
                0  << 7  | // EPD0
                24 << 0  // T0SZ
                );
    write_mair_el1(0x0); // Memory attribute register - whole memory is device nGnRnE
    dump_mmu();
    printk("start mmu\n");
    asm("dmb sy");
    asm("dsb sy");
    asm("isb");
//    asm("tlbi alle1"); // invalidate tlb1
    write_sctlr_el1((read_sctlr_el1() & ~4) | 0x1); // Enable MMUm disable cache
    asm("isb");

}
