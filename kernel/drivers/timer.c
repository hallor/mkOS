#include <stdint.h>
#include "asm.h"
#include "arch.h"
#include "timer.h"
#include "printk.h"
#include "config.h"
#include "gic.h"


#define dump_reg(X) do { uint64_t reg; MRS(X, reg); dbg(#X" = %#x (%d)\n", reg, reg); } while (0)

static uint64_t timer_frequency = 0;

void timer_init(void)
{
    uint64_t reg;
    MRS(CNTFRQ_EL0, timer_frequency);
    reg = timer_frequency / CONFIG_TIMER_FREQ;
    MSR(CNTP_CVAL_EL0, 0);
    MSR(CNTP_TVAL_EL0, reg);
    MSR(CNTP_CTL_EL0, 0x1); // enable interrupts
    gic_enable_interrupts(); // enable interrupts
    info("Initialized timer. Timer frequency: %d, HZ=%d\n", timer_frequency, CONFIG_TIMER_FREQ);
}


void timer_dump(void)
{
    dump_reg(CNTFRQ_EL0);
    dump_reg(CNTKCTL_EL1);
    dump_reg(CNTP_CTL_EL0);
    dump_reg(CNTP_CVAL_EL0);
    dump_reg(CNTP_TVAL_EL0);
    dump_reg(CNTPCT_EL0);
}
