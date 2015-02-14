#ifndef ASM_H
#define ASM_H

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

#ifdef __ASSEMBLY__
// Stores current context at address pointed by x9
.macro el0_context_store
    str x9, [sp, #-8]! // save x9
    ldr x9, =current
    ldr x9, [x9] // dereference current
    stp x0, x1, [x9], #16
    stp x2, x3, [x9], #16
    stp x4, x5, [x9], #16
    stp x6, x7, [x9], #16
    ldr x7, [sp], #8 // saved x9
    stp x8, x7, [x9], #16
    stp x10, x11, [x9], #16
    stp x12, x13, [x9], #16
    stp x14, x15, [x9], #16
    stp x16, x17, [x9], #16
    stp x18, x19, [x9], #16
    stp x20, x21, [x9], #16
    stp x22, x23, [x9], #16
    stp x24, x25, [x9], #16
    stp x26, x27, [x9], #16
    stp x28, x29, [x9], #16
    mrs x10, SP_EL0 // SP
    stp x30, x10, [x9], #16 // LR and SP from user

    mrs x10, SPSR_EL1 // CPSR from User
    mrs x11, ELR_EL1 // PC (eret address)
    stp x10, x11, [x9], #16

    mov x9, xzr // clean x9, x10, x11
    mov x10, xzr
    mov x11, xzr
.endm

// stores current context as stack variable
.macro el1_context_store
    sub sp, sp, #272
    stp x0, x1, [sp], #16
    stp x2, x3, [sp], #16
    stp x4, x5, [sp], #16
    stp x6, x7, [sp], #16
    stp x8, x9, [sp], #16
    stp x10, x11, [sp], #16
    stp x12, x13, [sp], #16
    stp x14, x15, [sp], #16
    stp x16, x17, [sp], #16
    stp x18, x19, [sp], #16
    stp x20, x21, [sp], #16
    stp x22, x23, [sp], #16
    stp x24, x25, [sp], #16
    stp x26, x27, [sp], #16
    stp x28, x29, [sp], #16
    add x10, sp, #32 // original SP
    stp x30, x10, [sp], #16 // LR and SP from user

    mrs x10, SPSR_EL1 // old CPSR
    mrs x11, ELR_EL1 // old PC (eret address)
    stp x10, x11, [sp], #16
    sub sp, sp, #272 // new top of stack
.endm

.macro el1_context_drop
    add sp, sp, #272 // throw away cpu_ctx structure
.endm

.macro el1_context_push
    stp x29, x30, [sp, #-16]!
    stp x27, x28, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x19, x20, [sp, #-16]!
    stp x17, x18, [sp, #-16]!
    stp x15, x16, [sp, #-16]!
    stp x13, x14, [sp, #-16]!
    stp x11, x12, [sp, #-16]!
    stp x9, x10, [sp, #-16]!
    stp x7, x8, [sp, #-16]!
    stp x5, x6, [sp, #-16]!
    stp x3, x4, [sp, #-16]!
    stp x1, x2, [sp, #-16]!
    stp xzr, x0, [sp, #-16]!
    mrs x0, SPSR_EL1
    mrs x1, ELR_EL1
    stp x1, x0, [sp, #-16]!
.endm

.macro el1_context_pop_and_eret
    ldp x1, x0, [sp], #16
    msr SPSR_EL1, x0
    msr ELR_EL1, x1
    ldp xzr, x0, [sp], #16
    ldp x1, x2, [sp], #16
    ldp x3, x4, [sp], #16
    ldp x5, x6, [sp], #16
    ldp x7, x8, [sp], #16
    ldp x9, x10, [sp], #16
    ldp x11, x12, [sp], #16
    ldp x13, x14, [sp], #16
    ldp x15, x16, [sp], #16
    ldp x17, x18, [sp], #16
    ldp x19, x20, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x27, x28, [sp], #16
    ldp x29, x30, [sp], #16
    eret
.endm

#warning TODO: el1_context_restore_and_eret + check if stack not smashed in context_store

// restores current context from current and returns from exception
.macro el0_context_restore_and_eret
    ldr x9, =current
    ldr x9, [x9] // get current structure
    ldp x0, x1, [x9], #16
    ldp x2, x3, [x9], #16
    ldp x4, x5, [x9], #16
    ldp x6, x7, [x9], #16
    ldp x8, xzr, [x9], #16 // throw away x9
    ldp x10, x11, [x9], #16
    ldp x12, x13, [x9], #16
    ldp x14, x15, [x9], #16
    ldp x16, x17, [x9], #16
    ldp x18, x19, [x9], #16
    ldp x20, x21, [x9], #16
    ldp x22, x23, [x9], #16
    ldp x24, x25, [x9], #16
    ldp x26, x27, [x9], #16
    ldp x28, x29, [x9], #16
    ldp x30, x10, [x9], #16 // LR and SP
    msr SP_EL0, x10

    ldp x10, x11, [x9], #16 // CPSR and PC
    msr SPSR_EL1, x10
    msr ELR_EL1, x11

    mov x9, xzr // clean x9, x10, x11
    mov x10, xzr
    mov x11, xzr
    eret // return from exception
.endm
#else // __ASSEMBLY__

#define MRS(S, V) asm("mrs %0,"#S"\n" : "=r"(V))
#define MSR(S, V) asm("msr "#S",%0\n" : :"r"(V))

#endif
#endif // ASM_H
