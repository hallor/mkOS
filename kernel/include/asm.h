#ifndef ASM_H
#define ASM_H
/*
 * This macro verifies that the a given vector doesn't exceed the
 * architectural limit of 32 instructions. This is meant to be placed
 * immedately after the last instruction in the vector. It takes the
 * vector entry as the parameter
 */
.macro check_vector_size since
  .if (. - \since) > (32 * 4)
    .error "Vector exceeds 32 instructions"
  .endif
.endm

.macro ctx_store
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
	str x0, [sp, #-8]!
	mrs x2, elr_el1
	mrs x1, spsr_el1
	stp x1, x2, [sp, #-16]!
	mrs x0, esr_el1
	mrs x1, elr_el1
	mrs x2, spsr_el1
.endm

.macro ctx_restore
	ldp x1, x2, [sp], #-16
	msr elr_el1, x2
	msr spsr_el1, x1
	ldr x0, [sp], #-8
	ldp x1, x2, [sp], #-16
	ldp x3, x4, [sp], #-16
	ldp x5, x6, [sp], #-16
	ldp x7, x8, [sp], #-16
	ldp x9, x10, [sp], #-16
	ldp x11, x12, [sp], #-16
	ldp x13, x14, [sp], #-16
	ldp x15, x16, [sp], #-16
	ldp x17, x18, [sp], #-16
	ldp x19, x20, [sp], #-16
	ldp x21, x22, [sp], #-16
	ldp x23, x24, [sp], #-16
	ldp x25, x26, [sp], #-16
	ldp x27, x28, [sp], #-16
	ldp x29, x30, [sp], #-16
.endm

#endif // ASM_H
