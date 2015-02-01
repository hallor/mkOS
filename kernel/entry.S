/*
  Copyright (C) 2015 Mateusz Kulikowski <mateusz.kulikowski@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asm.h"

.global start
start:
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

// Check, if we're in supervisor mode
mov x0, #1
mrs x1, CurrentEL
lsr x1, x1, #2
sub x0, x0, x1
cbnz x0, panic_mode

// initialize exception vector
adr x0, exception_vector
msr VBAR_EL1, x0

// initialize kernel
ldr x0, =init
bl puts
bl init_kernel

// start userspace
ldr x0, =user
bl puts
el0_context_restore_and_eret

// kernel finalized
ldr x0, =end
bl puts
1:
wfi
b 1b

panic_mode:
	ldr x0, =notsvc
	mrs x1, CurrentEL
	lsr x1, x1, #2
	bl putreg
	b .

welcome: .asciz "mkOS 1.0\n"
notsvc: .asciz "Panic: CPU not running in EL1 mode. CurrentEL"
init: .asciz "start kernel...\n"
user: .asciz "resume userspace...\n"
end: .asciz "kernel stopped\n"
