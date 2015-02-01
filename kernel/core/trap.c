#include "kern_console.h"
#include "util.h"
#include "syscalls.h"
#include "task.h"

void translate_exception_syndrome(uint64_t esr)
{
    int ec = esr >> 26;
    int il = esr >> 25 & 0x1;
    int iss = esr & 0x1FFFFFF;
    puts("Exception syndrome processing:\n");
    if (il)
        puts("Trapped instruction was 32-bit\n");
    else
        puts("Trapped instruction was 16-bit\n");
    switch (ec)
    {
    case 0x0: puts("EC: Unknown reason\n"); break;
    case 0x1: puts("EC: WFI or WFE\n"); break;
    case 0x3: puts("EC: MCR or MRC access to cp15\n"); break;
    case 0x4: puts("EC: MCRR or MRRC access to cp15\n"); break;
    case 0x5: puts("EC: MCR or MRC access to cp14\n"); break;
    case 0x6: puts("EC: LDC or STC access to cp14\n"); break;
    case 0x7: puts("EC: Access to disabled SIMD or VFP \n"); break;
    case 0x8: puts("EC: MRC or MCR access to cp10\n"); break;
    case 0xC: puts("EC: MRRC to cp14\n"); break;
    case 0xE: puts("EC: Illegal Execution State\n"); break;
    case 0x11: puts("EC: SVC from aa32\n"); break;
    case 0x12: puts("EC: HVC from aa32\n"); break;
    case 0x13: puts("EC: SMC from aa32\n"); break;
    case 0x15: puts("EC: SVC from aa64\n"); break;
    case 0x16: puts("EC: HVC from aa64\n"); break;
    case 0x17: puts("EC: SMC from aa64\n"); break;
    case 0x18: puts("EC: MSR, MRS or System instruction\n"); break;
    case 0x1F: puts("EC: Impl. defined exception to EL3\n"); break;
    case 0x20: puts("EC: Instruction Abort from lower ex level\n"); break;
    case 0x21: puts("EC: Instruction Abort from current ex level\n"); break;
    case 0x22: puts("EC: Misaligned PC exception\n"); break;
    case 0x24: puts("EC: DABT from lower exception level\n"); break;
    case 0x25: puts("EC: DABT from current exception level\n"); break;
    case 0x26: puts("EC: Stack Pointer Alignment exception\n"); break;
    case 0x28: puts("EC: Floating-point exception1\n"); break;
    case 0x2C: puts("EC: Floating-point exception2\n"); break;
    case 0x2F: puts("EC: SError interrupt\n"); break;
    case 0x30: puts("EC: Breakpoint from lower exception level\n"); break;
    case 0x31: puts("EC: Breakpoint from current exception level\n"); break;
    case 0x32: puts("EC: Step exception from lower exception level\n"); break;
    case 0x33: puts("EC: Step exception from current exception level\n"); break;
    case 0x34: puts("EC: Watchpoint exception from lower exception level\n"); break;
    case 0x35: puts("EC: Watchpoint exception from current exception level\n"); break;
    case 0x38: puts("EC: BKPT instruction from aa32\n"); break;
    case 0x3a: puts("EC: Vector catch exception from aa32\n"); break;
    case 0x3C: puts("EC: BRK instruction from aa64\n"); break;
    default:
        puts("EC: unknown\n");
        putreg("EC", ec);
    }

    putreg("IL", il);
    putreg("ISS", iss);
}

void bad_sync_from_el0(uint64_t esr)
{
    int ec = esr >> 26;

    if (ec == 0x15) {// SVC from aa64
        int imm = esr & 0x1FFFFFF; // imm
        puts("SVC @ PC "); puthex(current->ctx.pc); puts(" current "); puts(current->name); puts("\n");
        do_syscall(imm, current);
    } else {
        int i;
        puts("Exception: ");
        puts(__FUNCTION__);
        puts("\n");
        putreg("ESR: ", esr);
        translate_exception_syndrome(esr);

        puts("Task:\n");
        putreg("ELR: ", current->ctx.lr);
        putreg("CPSR: ", current->ctx.spsr);
        putreg("SP:", current->ctx.sp);
        putreg("PC:", current->ctx.pc);
        for (i=0; i<30; ++i)
            putreg("REG:", current->ctx.gpr[i]);
    }
    puts("rfe\n");
}

void bad_sync(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
    translate_exception_syndrome(esr);
}
void bad_irq(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
}
void bad_fiq(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
}
void bad_error(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
    translate_exception_syndrome(esr);
}
void do_sync(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
    translate_exception_syndrome(esr);
}
void do_irq(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
}
void do_fiq(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
}
void do_error(uint64_t esr)
{
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR: ", esr);
    translate_exception_syndrome(esr);
}
