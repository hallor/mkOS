//int uart_get(void);
.global uart_get
uart_get:
ldr x1, =0x1c090000
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
ldr x1, =0x1c090000
1:
	nop
	ldr x2, [x1, #0x18]
	and x2, x2, #0x20
	cbnz x2, 1b
and x0, x0, #0xFF
str x0, [x1]
ret
