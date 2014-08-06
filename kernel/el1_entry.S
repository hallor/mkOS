.global el1_start
el1_start:
// setup stack pointer for kernel
ldr x0, =_stack_top
mov sp, x0

// zero bss
ldr x0, =_sbss
ldr x1, =_ebss
sub x2, x0, x1
cbz x2, 2f
mov x2, #0

1:
	cmp x1, x0
	b.ge 2f
	str x2, [x1], #8
	b 1b

2:
ldr x0, =welcome
bl puts

// initialize exception vector
adr x0, el1_vector
msr VBAR_EL1, x0
// Configure state
mrs x0, DAIF
orr x0, x0, #0x3C0 // mask everything


// debug print
ldr x0, =cell
mrs x1, CurrentEL
lsr x1, x1, #2
bl putreg

// initialize kernel
ldr x0, =init
bl puts
bl el1_init

// initialize userspace

// start userspace
ldr x0, =user
bl puts


.global ctx_switch
ctx_switch:
ldr x0,=0x80100028 // setup LR to entry point
msr ELR_EL1, x0

ldr x0, =_user_stack_top // setup EL0 SP
msr SP_EL0, x0

mov x0, #0x3C0 // setup EL0 SPSR
msr SPSR_EL1, x0
ldr x0, =user2
bl puts
eret // Launch userspace


// kernel finalized
ldr x0, =end
bl puts
1:
wfe
b 1b

welcome: .asciz "mkOS 1.0\n"
init: .asciz "start kernel...\n"
user: .asciz "start userspace...\n"
user2: .asciz "branch userspace...\n"
end: .asciz "kernel stopped\n"
cell: .asciz "CurrentEL"
