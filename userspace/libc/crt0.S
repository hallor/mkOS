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

// Entry point
.global _start
_start:
// write task # to task
mov x28, x0 // save tid
adr x1, ex
add x0, x0, #'0'
strb w0, [x1, #28] // store byte
adr x1, ex2
strb w0, [x1, #27] // store byte

// print it
adr x0, ex
svc #0 // print
svc #2 // yield
cbnz x28, 1f
svc #2 // yield again for tid 0
1:
adr x0, ex2
svc #0 // print
svc #2 // yield

svc #1 // exit

1:
wfi
b 1b

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

ex: .asciz "Hello from userspace @ Task #.\n"
ex2: .asciz "Bye from userspace @ Task #.\n"
