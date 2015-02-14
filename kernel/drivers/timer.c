#include <stdint.h>
#include "asm.h"
#include "arch.h"
#include "timer.h"
#include "printk.h"
#include "config.h"
#include "gic.h"
#include "task.h"

#define dump_reg(X) do { uint64_t reg; MRS(X, reg); dbg(#X" = %#x (%d)\n", reg, reg); } while (0)

static uint64_t timer_frequency = 0;

static void timer_handler(void * p)
{
    info("Timer interrupt!\n");
    MSR(CNTP_CTL_EL0, 0x0);
    MSR(CNTP_CVAL_EL0, 0);
    MSR(CNTP_TVAL_EL0, timer_frequency / CONFIG_TIMER_FREQ);
    MSR(CNTP_CTL_EL0, 0x1); // enable interrupts from timer
    task_next(); // context switch
}

void timer_init(void)
{
    uint64_t reg;
    MRS(CNTFRQ_EL0, timer_frequency);
    reg = timer_frequency / CONFIG_TIMER_FREQ;
    MSR(CNTP_CVAL_EL0, 0);
    MSR(CNTP_TVAL_EL0, reg);
    gic_register_irq(30, timer_handler, 0); // timer irq??
    MSR(CNTP_CTL_EL0, 0x1); // enable interrupts from timer
    info("Initialized timer. Timer frequency: %d, HZ=%d\n", timer_frequency, CONFIG_TIMER_FREQ);
}

void timer_restart(void)
{
    MSR(CNTP_CTL_EL0, 0x0);
    MSR(CNTP_CVAL_EL0, 0);
    MSR(CNTP_TVAL_EL0, timer_frequency / CONFIG_TIMER_FREQ);
    MSR(CNTP_CTL_EL0, 0x1); // enable interrupts from timer
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
