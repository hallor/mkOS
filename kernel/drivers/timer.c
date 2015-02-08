#include <stdint.h>
#include "arch.h"
#include "timer.h"
#include "printk.h"
#include "config.h"

#define MRS(S, V) asm("mrs %0,"#S"\n" : "=r"(V))

#define MSR(S, V) asm("msr "#S",%0\n" : :"r"(V))

static uint64_t timer_frequency = 0;

#define dump_reg(X) do { uint64_t reg; MRS(X, reg); dbg(#X" = %#x (%d)\n", reg, reg); } while (0)

#define GIC_BASE 0x2c001000

static void dump_gic(void)
{
    uint32_t *regs = (void*)GIC_BASE;
    int i;

    for (i=0; i<10; ++i)
        info("GIC[%d] %#x\n", i, (unsigned)regs[i]);
    info("GIC %#x\n", regs[0xFC >> 2]);
}

void timer_init(void)
{
    uint64_t reg;
    MRS(CNTFRQ_EL0, timer_frequency);
    reg = timer_frequency / CONFIG_TIMER_FREQ;
    MSR(CNTP_CVAL_EL0, 0);
    MSR(CNTP_TVAL_EL0, reg);
    MSR(CNTP_CTL_EL0, 0x1); // enable interrupts
    info("Initializing timer. Fimer frequency: %d, HZ=%d\n", timer_frequency, CONFIG_TIMER_FREQ);
    timer_dump();
    dump_reg(CurrentEL);
    dump_reg(DAIF);
    asm("msr daifclr, #0x2\n");
//    MSR(DAIF, 0x2);
    dump_reg(DAIF);

    dump_gic();
//    dump_reg(ICC_SRE_EL1);

    while(1) {
        MRS(CNTP_CTL_EL0, reg);
        if (reg & ~ 0x1)
            break;
//        asm("wfi");
//        dump_reg(CNTP_TVAL_EL0);
    }
//    dump_reg(ICC_SRE_EL1);
    info("Timer zadzwonil.\n");
}


void timer_dump(void)
{
    uint64_t reg;
    dump_reg(CNTFRQ_EL0);
    dump_reg(CNTKCTL_EL1);
    dump_reg(CNTP_CTL_EL0);
    dump_reg(CNTP_CVAL_EL0);
    dump_reg(CNTP_TVAL_EL0);
    dump_reg(CNTPCT_EL0);
}
