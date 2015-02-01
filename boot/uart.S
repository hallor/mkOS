#include "config.h"
//int uart_get(void);
.global uart_get
uart_get:
ldr x1, =CONFIG_UART0_ADDR
1:
    nop
    ldr x0, [x1, #0x18]
    and x0, x0, #0x10
    cbnz x0, 1b
ldr x0, [x1]
ret


//void uart_put(int c);
.global uart_put
uart_put:
ldr x1, =CONFIG_UART0_ADDR
1:
    nop
    ldr x2, [x1, #0x18]
    and x2, x2, #0x20
    cbnz x2, 1b
and x0, x0, #0xFF
str x0, [x1]
ret

// void uart_puts (char*)
.global uart_puts
uart_puts:
str x30, [sp, #-8]!
mov x20, x0
1:
    ldrb w0, [x20], #0x1
    cbz w0, 2f
    bl uart_put
    b 1b
2:
ldr x30, [sp], #8
ret
