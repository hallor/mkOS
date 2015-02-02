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

#include "printk.h"
#include "kern_console.h"
#include "util.h"
#include "syscalls.h"
#include "task.h"

void translate_exception_syndrome(uint64_t esr)
{
    int ec = esr >> 26;
    int il = esr >> 25 & 0x1;
    int iss = esr & 0x1FFFFFF;
    printk("IL: %#x (%d-bit instruction) ISS: %#x EC: %#x EC_Explanation: ", il, il ? 32 : 16, iss, ec);
    switch (ec)
    {
    case 0x0: puts("Unknown reason\n"); break;
    case 0x1: puts("WFI or WFE\n"); break;
    case 0x3: puts("MCR or MRC access to cp15\n"); break;
    case 0x4: puts("MCRR or MRRC access to cp15\n"); break;
    case 0x5: puts("MCR or MRC access to cp14\n"); break;
    case 0x6: puts("LDC or STC access to cp14\n"); break;
    case 0x7: puts("Access to disabled SIMD or VFP \n"); break;
    case 0x8: puts("MRC or MCR access to cp10\n"); break;
    case 0xC: puts("MRRC to cp14\n"); break;
    case 0xE: puts("Illegal Execution State\n"); break;
    case 0x11: puts("SVC from aa32\n"); break;
    case 0x12: puts("HVC from aa32\n"); break;
    case 0x13: puts("SMC from aa32\n"); break;
    case 0x15: puts("SVC from aa64\n"); break;
    case 0x16: puts("HVC from aa64\n"); break;
    case 0x17: puts("SMC from aa64\n"); break;
    case 0x18: puts("MSR, MRS or System instruction\n"); break;
    case 0x1F: puts("Impl. defined exception to EL3\n"); break;
    case 0x20: puts("Instruction Abort from lower ex level\n"); break;
    case 0x21: puts("Instruction Abort from current ex level\n"); break;
    case 0x22: puts("Misaligned PC exception\n"); break;
    case 0x24: puts("DABT from lower exception level\n"); break;
    case 0x25: puts("DABT from current exception level\n"); break;
    case 0x26: puts("Stack Pointer Alignment exception\n"); break;
    case 0x28: puts("Floating-point exception1\n"); break;
    case 0x2C: puts("Floating-point exception2\n"); break;
    case 0x2F: puts("SError interrupt\n"); break;
    case 0x30: puts("Breakpoint from lower exception level\n"); break;
    case 0x31: puts("Breakpoint from current exception level\n"); break;
    case 0x32: puts("Step exception from lower exception level\n"); break;
    case 0x33: puts("Step exception from current exception level\n"); break;
    case 0x34: puts("Watchpoint exception from lower exception level\n"); break;
    case 0x35: puts("Watchpoint exception from current exception level\n"); break;
    case 0x38: puts("BKPT instruction from aa32\n"); break;
    case 0x3a: puts("Vector catch exception from aa32\n"); break;
    case 0x3C: puts("BRK instruction from aa64\n"); break;
    default:
        puts("Unknown\n");
    }

}

void trap_sync_from_el0_aa64(uint64_t esr)
{
    int ec = esr >> 26;

    if (ec == 0x15) {// SVC from aa64
        int imm = esr & 0x1FFFFFF; // imm
        dbg("svc at %s tid %d pc %#llx\n", current->name, current->tid, current->ctx.pc);
        do_syscall(imm, current);
    } else {
        int i;
        wrn("Exception in %s tid %d\n", current->name, current->tid);
        translate_exception_syndrome(esr);

        printk("ELR: %#llx CPSR: %#llx SP: %#llx PC: %#llx LR: %#llx\n",
            current->ctx.lr, current->ctx.spsr, current->ctx.sp, current->ctx.pc, current->ctx.lr);
        for (i=0; i<30; ++i) {
            printk("X%2i: %#16llx ", i, current->ctx.gpr[i]);
            if (i % 4 == 3)
                puts("\n");
        }
        puts("\n");
        panic("No handler designed.\n");
    }
}

void trap_irq_from_el0_aa64(uint64_t esr)
{
    wrn("IRQ in %s tid %d. ESR: %#llx\n", current->name, current->tid, esr);
}

void trap_fiq_from_el0_aa64(uint64_t esr)
{
    wrn("FIQ in %s tid %d. ESR: %#llx\n", current->name, current->tid, esr);
}

void trap_serror_from_el0_aa64(uint64_t esr)
{
    wrn("SError in %s tid %d. ESR: %#llx\n", current->name, current->tid, esr);
    translate_exception_syndrome(esr);
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
    uint64_t pc;
    puts("Exception: ");
    puts(__FUNCTION__);
    puts("\n");
    putreg("ESR", esr);
    asm("mrs %0, ELR_EL1": "=r"(pc));
    putreg("PC", pc);
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
