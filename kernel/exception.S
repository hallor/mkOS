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

#include "include/asm.h"
/*
 * Arm Procedure call register usage:
 * r0..r7 - parameter/result registers
 * r8 indirect result location register
 * r9..r15 - temporary registers
 * r16, r17 - IP0/1
 * r18 - platform reg.
 * r19..r28 - callee saved
 * r29 - fp
 * r30 - lr
 *
 * subroutine must preserve r19-r29 + sp
 */
.macro exception_params
	mrs x0, esr_el1
.endm

.section	.vectors, "ax"; .align 11
.global exception_vector
exception_vector:

// Vectors 0x0 - 0x180 -> from current EL with SP_EL0
.align  7
vector_from_el1_sp0_sync:
	b handler_from_el1_sp0_sync
.align  7
vector_from_el1_sp0_irq:
	b handler_from_el1_sp0_irq
.align  7
vector_from_el1_sp0_fiq:
	b handler_from_el1_sp0_fiq
.align  7
vector_from_el1_sp0_serror:
	b handler_from_el1_sp0_serror

// Vectors 0x200 - 0x380 -> from current EL with SP_EL1
.align  7
vector_from_el1_sp1_sync:
	b handler_from_el1_sp1_sync
.align  7
vector_from_el1_sp1_irq:
	b handler_from_el1_sp1_irq
.align  7
vector_from_el1_sp1_fiq:
	b handler_from_el1_sp1_fiq
.align  7
vector_from_el1_sp1_serror:
	b handler_from_el1_sp1_serror

// Vectors 0x400 - 0x580 -> from lower EL in AA64
.align  7
vector_from_el0_aa64_sync:
	b handler_from_el0_aa64_sync
.align  7
vector_from_el0_aa64_irq:
	b handler_from_el0_aa64_irq
.align  7
vector_from_el0_aa64_fiq:
	b handler_from_el0_aa64_fiq
.align  7
vector_from_el0_aa64_serror:
	b handler_from_el0_aa64_serror

// Vectors 0x600 - 0x780 -> from lower EL in AA32
.align  7
vector_from_el0_aa32_sync:
	b handler_from_el0_aa32_sync
.align  7
vector_from_el0_aa32_irq:
	b handler_from_el0_aa32_irq
.align  7
vector_from_el0_aa32_fiq:
	b handler_from_el0_aa32_fiq
.align  7
vector_from_el0_aa32_serror:
	b handler_from_el0_aa32_serror


// full handlers handlers
// from EL1 with SP0
handler_from_el1_sp0_sync:
handler_from_el1_sp0_irq:
handler_from_el1_sp0_fiq:
handler_from_el1_sp0_serror:
	adr x0, not_supported_sp0_msg
	b panic
// from EL1 with SP1
handler_from_el1_sp1_sync:
	el1_context_store
	mov x1, sp // save cpu_ctx address as arg2
	mrs x0, esr_el1 // arg1
	str x30, [sp, #-8]!
	bl trap_sync_from_el1_sp1
	ldr x30, [sp], #8
	el1_context_drop
	adr x0, system_halted
	b panic
handler_from_el1_sp1_irq:
handler_from_el1_sp1_fiq:
	adr x0, kernel_crashed
	b panic
handler_from_el1_sp1_serror:
	el1_context_store
	mov x1, sp // save cpu_ctx address as arg2
	mrs x0, esr_el1 // arg1
	str x30, [sp, #-8]!
	bl trap_serror_from_el1_sp1
	ldr x30, [sp], #8
	el1_context_drop
	adr x0, system_halted
	b panic
// from EL0 AA64
handler_from_el0_aa64_sync:
	el0_context_store
	exception_params
	bl trap_sync_from_el0_aa64
	el0_context_restore_and_eret
handler_from_el0_aa64_irq:
	el0_context_store
	exception_params
	bl trap_irq_from_el0_aa64
	el0_context_restore_and_eret
handler_from_el0_aa64_fiq:
	el0_context_store
	exception_params
	bl trap_fiq_from_el0_aa64
	el0_context_restore_and_eret
handler_from_el0_aa64_serror:
	el0_context_store
	exception_params
	bl trap_serror_from_el0_aa64
	el0_context_restore_and_eret
// from EL0 AA32
handler_from_el0_aa32_sync:
handler_from_el0_aa32_irq:
handler_from_el0_aa32_fiq:
handler_from_el0_aa32_serror:
	adr x0, not_supported_aa32_msg
	b panic


panic:
cbz x0, 1f
bl puts
1:
wfe
b 1b
not_supported_sp0_msg: .asciz "Panic: Unexpected exception from EL1 using sp0. System halted.\n";
kernel_crashed: .asciz "Panic: Unexpected exception from EL1 using SP1. System halted.\n";
not_supported_aa32_msg: .asciz "Panic: Unexpected exception from 32-bit EL0. System halted.\n";
system_halted: .asciz "System Halted.\n";
