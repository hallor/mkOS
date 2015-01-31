#ifndef ASM_H
#define ASM_H
/*
 * This macro verifies that the a given vector doesn't exceed the
 * architectural limit of 32 instructions. This is meant to be placed
 * immedately after the last instruction in the vector. It takes the
 * vector entry as the parameter
 */
#ifdef __ASSEMBLY__
.macro check_vector_size since
  .if (. - \since) > (32 * 4)
    .error "Vector exceeds 32 instructions"
  .endif
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
#endif // __ASSEMBLY__

#endif // ASM_H
