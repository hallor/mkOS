#include "asm.h"
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

// Stores current context in current structure
.macro el0_context_store
    ldr x9, =current // throw away x9
    ldr x9, [x9] // dereference current
    stp x0, x1, [x9], #16
    stp x2, x3, [x9], #16
    stp x4, x5, [x9], #16
    stp x6, x7, [x9], #16
    stp x8, xzr, [x9], #16  // throw away x9
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

.section	.vectors, "ax"; .align 11
.global el1_vector
el1_vector:

// Vectors 0x0 - 0x180 -> from current EL with SP_EL0
.align  7
SynchronousExceptionSP0:
exception_params
bl bad_sync
eret

.align  7
IrqSP0:
exception_params
	bl bad_irq
	eret

.align  7
FiqSP0:
exception_params
	bl bad_fiq
	eret

.align  7
SErrorSP0:
exception_params
	bl bad_error
	eret

// Vectors 0x200 - 0x380 -> from current EL with SP_ELx
.align  7
SynchronousExceptionSPx:
exception_params
	bl do_sync
	b 1f

.align  7
IrqSPx:
exception_params
	bl do_irq
	eret

.align  7
FiqSPx:
exception_params
	bl do_fiq
	eret

.align  7
SErrorSPx:
exception_params
	bl do_error
	b 1f

// Vectors 0x400 - 0x580 -> from lower EL in AA64
.align  7
	SynchronousException_lowerEL_AA64:
	el0_context_store
	exception_params
	bl bad_sync_from_el0
    el0_context_restore_and_eret

.align  7
Irq_lowerEL_AA64:
	exception_params
	bl bad_irq
	eret

.align  7
Fiq_lowerEL_AA64:
	exception_params
	bl bad_fiq
	eret

.align  7
SError_lowerEL_AA64:
	exception_params
	bl bad_error
	eret

// Vectors 0x600 - 0x780 -> from lower EL in AA32
.align  7
SynchronousException_lowerEL_AA32:
	exception_params
	bl bad_sync
	eret

.align  7
Irq_lowerEL_AA32:
	exception_params
	bl bad_irq
	eret

.align  7
Fiq_lowerEL_AA32:
	exception_params
	bl bad_fiq
	eret

.align  7
SError_lowerEL_AA32:
	exception_params
	bl bad_error
	eret

1:
wfe
b 1b


ex: .asciz "Exception occured.\n"
esr: .asciz "ESR"
