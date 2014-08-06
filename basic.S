.global _start
_start:
ldr x0, arg // Load the lower 32-bits of DTB 
mov x1, xzr 
mov x2, xzr 
mov x3, xzr 
ldr x4, entry // Load the lower 32-bits of kernel entry 
br x4      // Jump to the kernel entry point 

entry:  .word
arg: .word
