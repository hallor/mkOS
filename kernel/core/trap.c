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
#include "gic.h"
#include "timer.h"

static void parse_dabt(uint32_t iss)
{
    // TODO: parse other fields
    unsigned dfsc = iss & 0x3F;
    printk("ISS: %#x: DFSC: %#x Explanation: ", iss, dfsc);
    switch(dfsc)
    {
    case 0: puts("Address size fault, zeroth level of translation or ttbr\n"); break;
    case 1: puts("Address size fault, first level\n"); break;
        case 2: puts("Address size fault, second level\n"); break;
        case 3: puts("Address size fault, third level\n"); break;
        case 4: puts("Translation fault, zeroth level\n"); break;
        case 5: puts("Translation fault, first level\n"); break;
        case 6: puts("Translation fault, second level\n"); break;
        case 7: puts("Translation fault, third level\n"); break;
        case 9: puts("Access flag fault, first level\n"); break;
        case 10: puts("Access flag fault, second level\n"); break;
        case 11: puts("Access flag fault, third level\n"); break;
        case 13: puts("Permission fault, first level\n"); break;
        case 14: puts("Permission fault, second level\n"); break;
        case 15: puts("Permission fault, thidr level\n"); break;
        // TODO: other
    default:
        puts("Unknown or not implemented DFSC\n");
    }
}

static void parse_iabt(uint32_t iss)
{
    parse_dabt(iss); // TODO: replace with proper parser

}

static void translate_exception_syndrome(uint64_t esr)
{
    int ec = esr >> 26;
    int il = esr >> 25 & 0x1;
    int iss = esr & 0x1FFFFFF;
    printk("ESR: %#x IL: %#x (%d-bit instruction) ISS: %#x EC: %#x EC_Explanation: ", esr, il, il ? 32 : 16, iss, ec);
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
    case 0x20: puts("Instruction Abort from lower ex level\n"); parse_iabt(iss); break;
    case 0x21: puts("Instruction Abort from current ex level\n"); parse_iabt(iss); break;
    case 0x22: puts("Misaligned PC exception\n"); break;
    case 0x24: puts("DABT from lower exception level\n"); parse_dabt(iss); break;
    case 0x25: puts("DABT from current exception level\n"); parse_dabt(iss); break;
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

static void dump_cpu_context(struct cpu_ctx * ctx)
{
    int i;
    printk("ELR: %#llx CPSR: %#llx SP: %#llx PC: %#llx LR: %#llx\n",
        ctx->lr, ctx->spsr, ctx->sp, ctx->pc, ctx->lr);
    for (i=0; i<30; ++i) {
        printk("X%2i: %#16llx ", i, ctx->gpr[i]);
        if (i % 4 == 3)
            puts("\n");
    }
}

void trap_irq_from_el1_aa64(uint64_t esr)
{
    wrn("IRQ in kernel mode. ESR: %#llx\n", esr);
    gic_handle_irq();
}


void trap_sync_from_el0_aa64(uint64_t esr)
{
    int ec = esr >> 26;

    if (ec == 0x15) {// SVC from aa64
        int imm = esr & 0x1FFFFFF; // imm
        dbg("svc at %s tid %d pc %#llx\n", current->name, current->tid, current->ctx.pc);
        do_syscall(imm, current);
    } else {
        wrn("Synchronous exception in %s tid %d\n", current->name, current->tid);
        translate_exception_syndrome(esr);
        dump_cpu_context(&current->ctx);
        puts("\n");
        panic("No handler designed.\n");
    }
}

void trap_irq_from_el0_aa64(uint64_t esr)
{
    wrn("IRQ in %s tid %d. ESR: %#llx\n", current->name, current->tid, esr);
    gic_handle_irq();
}

void trap_fiq_from_el0_aa64(uint64_t esr)
{
    wrn("FIQ in %s tid %d. ESR: %#llx\n", current->name, current->tid, esr);
    gic_handle_irq();

}

void trap_serror_from_el0_aa64(uint64_t esr)
{
    wrn("SError in %s tid %d. ESR: %#llx\n", current->name, current->tid, esr);
    translate_exception_syndrome(esr);
}

// Kernel crash handlers - they're different, as they will have cpu context on stack
void trap_sync_from_el1_sp1(uint64_t esr, struct cpu_ctx * ctx)
{
    err("sync exception from kernel mode.\n");
    translate_exception_syndrome(esr);
    dump_cpu_context(ctx);
    panic("Kernel Panic\n");
}

void trap_serror_from_el1_sp1(uint64_t esr, struct cpu_ctx * ctx)
{
    err("Serror exception from kernel mode.\n");
    translate_exception_syndrome(esr);
    dump_cpu_context(ctx);
    panic("Kernel Panic\n");
}
