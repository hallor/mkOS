#include <stdint.h>
#include "asm.h"
#include "config.h"
#include "gic.h"
#include "io.h"
#include "printk.h"
#include "kmalloc.h"

#define GICD_CTLR   0x0
#define GICD_TYPER  0x4
#define GICD_IIDR   0x8
#define GICD_IGROUPR(n)   (0x80 + (n) * 4)
#define GICD_ISENABLER(n) (0x100 + (n) * 4)
#define GICD_ICENABLER(n) (0x180 + (n) * 4)
#define GICD_ISPENDR(n)   (0x200 + (n) * 4)
#define GICD_ICPENDR(n)   (0x280 + (n) * 4)
#define GICD_ISACTIVER(n)  (0x300 + (n) * 4)
#define GICD_ICACTIVER(n)  (0x380 + (n) * 4)
#define GICD_IPRIORITYR(n)(0x400 + (n) * 4)
#define GICD_ITARGETSR(n) (0x800 + (n) * 4)
#define GICD_ICFGR(n)     (0xC00 + (n) * 4)

#define GICC_CTLR   0x0
#define GICC_PMR    0x4
#define GICC_BPR    0x8
#define GICC_IAR    0xC
#define GICC_EOIR   0x10
#define GICC_RPR    0x14
#define GICC_HPPIR  0x18
#define GICC_ABPR   0x1C
#define GICC_AIAR   0x20
#define GICC_AEOIR  0x24
#define GICC_AHPPIR 0x28
#define GICC_APR0   0xD0
#define GICC_APR1   0xD4
#define GICC_APR2   0xD8
#define GICC_APR3   0xDC
#define GICC_NSAPR0 0xE0
#define GICC_NSAPR1 0xE4
#define GICC_NSAPR2 0xE8
#define GICC_NSAPR3 0xEC
#define GICC_IIDR   0xFC
#define GICC_DIR    0x1000


#define GICD_REG(R) (CONFIG_GICD_CPU_BASE + R)
#define GICC_REG(R) (CONFIG_GICC_CPU_BASE + R)

#define gicd_read(r) ioread32(GICD_REG(r))
#define gicd_write(r, v) iowrite32(GICD_REG(r), v)
#define gicc_read(r) ioread32(GICC_REG(r))
#define gicc_write(r, v) iowrite32(GICC_REG(r), v)

struct gic_handler {
    irq_handler_t fcn;
    void * arg;
};

static struct gic_handler *irq_handlers = 0;
static unsigned irq_count = 0;

void gic_enable_interrupts(void)
{
    iowrite32(GICC_REG(GICC_CTLR), 1); // enable signaling of interrupts
}

void gic_disable_interrupts(void)
{
    iowrite32(GICC_REG(GICC_CTLR), 0); // disable signaling of interrupts
}

void gic_enable_irq(unsigned no)
{
    unsigned reg = no / 32;
    unsigned bit = no % 32;
    if (no >= irq_count)
        panic("No such interrupt - %d\n", no);
    gicd_write(GICD_ISENABLER(reg), 1 << bit);
}

void gic_disable_irq(unsigned no)
{
    unsigned reg = no / 32;
    unsigned bit = no % 32;
    if (no >= irq_count)
        panic("No such interrupt - %d\n", no);
    gicd_write(GICD_ICENABLER(reg), 1 << bit);
}

// called in interrupt context
void gic_handle_irq(void)
{
    unsigned irqno;
    while(1)
    {
        irqno = gicc_read(GICC_IAR);
        unsigned cpuid = irqno > 9;
        irqno &= 0x3FF;
        if (irqno == 0x3ff) // end of interrupts
            break;
        if (irqno >= irq_count || irq_handlers[irqno].fcn == 0) {
            err("Suprious interrupt %d\n", irqno);
            gic_dump();
            break;
        }
        irq_handlers[irqno].fcn(irq_handlers[irqno].arg);
        gicc_write(GICC_EOIR, irqno);
    }
}

void gic_register_irq(unsigned no, irq_handler_t fcn, void * parm)
{
    if (no >= irq_count) {
        wrn("Unable to register irq %d\n", no);
        return;
    }

    irq_handlers[no].fcn = fcn;
    irq_handlers[no].arg = parm;
    gic_enable_irq(no);
}

void gic_unregister_irq(unsigned no)
{
    if (no >= irq_count) {
        wrn("Unable to unregister irq %d\n", no);
        return;
    }
    gic_disable_irq(no);
    irq_handlers[no].fcn = 0;
    irq_handlers[no].arg = 0;
}


void gic_init(void)
{
    int i;
    info("Initializing gicd @%#llx gicc @%#llx\n", CONFIG_GICD_CPU_BASE, CONFIG_GICC_CPU_BASE);
    irq_count = gicd_read(GICD_TYPER) & 0xF;
    irq_count = 32 * (irq_count + 1);

    irq_handlers = kzalloc(irq_count * sizeof(struct gic_handler));
    if (!irq_handlers)
        panic("Failed to allocate space for handlers.\n");
    info("Will support %d interrupts.\n", irq_count);

    // Initialize distributor
    for (i=0; i<irq_count / 32; ++i) {
        gicd_write(GICD_IGROUPR(i), 0x0); // all for group 0
    }
    for (i=0; i<irq_count / 4; ++i) {
        gicd_write(GICD_IPRIORITYR(i), 0x00000000);
        gicd_write(GICD_ITARGETSR(i),  0xFFFFFFFF);
    }
    for (i=0; i<irq_count / 16; ++i)
        gicd_write(GICD_ICFGR(i), 0xAAAAAAAA);

    gicd_write(GICD_CTLR, 1); // enable distributor
    // Initialize CPU interface
    gicc_write(GICC_PMR, 0x1);
}

void gic_do(void)
{
    info("waiting for next irq\n");
    gic_dump();
    while (gicd_read(GICD_ISPENDR(0)) == 0) {

    }
    gic_dump();
    info("Interrupt!\n");
}

void gic_dump(void)
{
    unsigned i;
    printk("GICD_IIDR %#x GICD_TYPER %#x\n", gicd_read(GICD_IIDR), gicd_read(GICD_TYPER));
    printk("GICC_IIDR %#x GICC_IAR %#x\n", gicc_read(GICC_IIDR), gicc_read(GICC_IAR));

    printk("%d interrupts.\n", irq_count);
    printk("GICD_IPRIORITY = ");
    for (i=0; i<irq_count / 4; ++i)
        printk("0x%08x ", gicd_read(GICD_IPRIORITYR(i)));
    printk("\nGICD_ITARGETSR = ");
    for (i=0; i<irq_count / 4; ++i)
        printk("0x%08x ", gicd_read(GICD_ITARGETSR(i)));
    printk("\nGICD_ISENABLER = ");
    for (i=0; i<irq_count / 32; ++i)
        printk("0x%08x ", gicd_read(GICD_ISENABLER(i)));
    printk("\nGICD_ISPENDR   = ");
    for (i=0; i<irq_count / 32; ++i)
        printk("0x%08x ", gicd_read(GICD_ISPENDR(i)));
    printk("\nGICD_ISACTIVER = ");
    for (i=0; i<irq_count / 32; ++i)
        printk("0x%08x ", gicd_read(GICD_ISACTIVER(i)));
    printk("\n");
    printk("GICC_IAR 0x%08x GICC_HPPIR 0x%08x GICC_AIAR 0x%08x\n",
           gicc_read(GICC_IAR), gicc_read(GICC_HPPIR), gicc_read(GICC_AIAR));

}

