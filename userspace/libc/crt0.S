// Entry point
.global _start
_start:
// Get stack pointer address
ldr x0, =ex
mov x29, #55
svc #0
svc #1
b .
mov sp, x0

mov x0, #1 // 1 == puts
ldr x1, =ex
svc #0 // syscall

svc #42
b .
// setup stack pointer
//ldr x0, =_stack_top
//mov sp, x0

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
	b __libc_main
	svc #42
	b .

ex: .asciz "Hello from userspace.\n"
