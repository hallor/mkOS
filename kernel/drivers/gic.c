#include <stdint.h>
#include "asm.h"
#include "config.h"
#include "gic.h"
#include "io.h"
#include "printk.h"

#define GICD_CTLR   0x0
#define GICD_TYPER  0x4
#define GICD_IIDR   0x8
#define GICD_IGROUPR(n)   (0x80 + (n) * 4)
#define GICD_ISENABLER(n) (0x100 + (n) * 4)
#define GICD_ICENABLER(n) (0x180 + (n) * 4)
#define GICD_ISPENDR(n)   (0x200 + (n) * 4)
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


#define GICD_REG(R) (CONFIG_GIC_CPU_BASE + 0x1000 + R)
#define GICC_REG(R) (CONFIG_GIC_CPU_BASE + 0x2000 + R)

#define gicd_read(r) ioread32(GICD_REG(r))
#define gicd_write(r, v) iowrite32(GICD_REG(r), v)
#define gicc_read(r) ioread32(GICC_REG(r))
#define gicc_write(r, v) iowrite32(GICC_REG(r), v)

static unsigned gic_irq_cnt = 0;

void gic_enable_interrupts(void)
{
    iowrite32(GICC_REG(GICC_CTLR), 3); // enable signaling of interrupts
}

void gic_disable_interrupts(void)
{
    iowrite32(GICC_REG(GICC_CTLR), 0); // disable signaling of interrupts
}

void gic_dump(void)
{
  unsigned i;
  printk("GICD_IIDR %#x GICD_TYPER %#x\n", gicd_read(GICD_IIDR), gicd_read(GICD_TYPER));
  printk("GICC_IIDR %#x GICC_IAR %#x\n", gicc_read(GICC_IIDR), gicc_read(GICC_IAR));

  printk("%d interrupts.\n", gic_irq_cnt);
  printk("GICD_IPRIORITY = ");
  for (i=0; i<gic_irq_cnt / 32; ++i)
    printk("0x%08x ", gicd_read(GICD_IPRIORITYR(i)));
  printk("\nGICD_ITARGETSR = ");
  for (i=0; i<gic_irq_cnt / 32; ++i)
    printk("0x%08x ", gicd_read(GICD_ITARGETSR(i)));
  printk("\nGICD_ISENABLER = ");
  for (i=0; i<gic_irq_cnt / 32; ++i)
    printk("0x%08x ", gicd_read(GICD_ISENABLER(i)));
  printk("\nGICD_ISPENDR =   ");
  for (i=0; i<gic_irq_cnt / 32; ++i)
    printk("0x%08x ", gicd_read(GICD_ISPENDR(i)));
  printk("\nGICD_ISACTIVER = ");
  for (i=0; i<gic_irq_cnt / 32; ++i)
    printk("0x%08x ", gicd_read(GICD_ISACTIVER(i)));
  printk("\n");
  printk("GICC_IAR 0x%08x GICC_HPPIR 0x%08x GICC_AIAR 0x%08x\n",
         gicc_read(GICC_IAR), gicc_read(GICC_HPPIR), gicc_read(GICC_AIAR));

}


void gic_init()
{
    int i;
    info("Initializing gic @%#llx\n", CONFIG_GIC_CPU_BASE);
    gic_irq_cnt = gicd_read(GICD_TYPER) & 0xF;
    gic_irq_cnt = 32 * (gic_irq_cnt + 1);

    gic_dump();
    // Initialize distributor
    gicd_write(GICD_IGROUPR(0), 0xFFFFFFFF); // all for group 1
    gicd_write(GICD_ISENABLER(0), 0xFFFFFFFF); // enable all
    for (i=0; i<gic_irq_cnt / 4; ++i) {
      gicd_write(GICD_IPRIORITYR(i), 0xFFFFFFFF);
      gicd_write(GICD_ITARGETSR(i), 0xFFFFFFFF);
    }
    gicd_write(GICD_ICFGR(0), 0xAAAAAAAA);
    gicd_write(GICD_ICFGR(1), 0xAAAAAAAA);

    gicd_write(GICD_CTLR, 3); // enable
    // Initialize CPU interface
    gicc_write(GICC_PMR, 0);
    gicc_write(GICC_CTLR, 3); // enable

    gicd_write(GICD_ISACTIVER(0), 0xFFFFFFFF);
}
