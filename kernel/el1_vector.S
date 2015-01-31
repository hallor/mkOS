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
