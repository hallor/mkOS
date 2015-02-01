#include "config.h"
#include "image.h"
.macro puts string
ldr x0,=\string
bl uart_puts
.endm

.global start
start:
// setup stack pointer for bootloader
ldr x0, =_stack_top
mov sp, x0

// setup bss
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

puts welcome
bl main
puts pre_panic
puts panic
b .

welcome: .asciz "mkBOOT 1.0\n"
pre_panic: .asciz "Error: main() finished!\n"
panic: .asciz "BL panic!\n"

