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
#include "config.h"
#include "image.h"
.macro puts string
adr x0,\string
bl uart_puts
.endm

.global start
start:

// setup stack pointer for bootloader
ldr x0, =_stack_top
mov sp, x0

puts init

// copy code
puts reloc
nop
ldr x0, =__copy_start // TODO __copy_start
ldr x1, =__copy_end // TODO __copy_end
ldr x2, =CONFIG_NOR_CS0_START // copy-from-address

1:
    ldr x3, [x2], #8
    str x3, [x0], #8
    cmp x0, x1
    b.lt 1b
puts jump
ldr x0, =in_ram
br x0
b do_panic

in_ram:

puts bssc
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
do_panic:
puts panic
b .

init: .asciz "I"
bssc: .asciz "B"
reloc: .asciz "R"
jump: .asciz "J"
welcome: .asciz "mkBOOT 1.0\n"
pre_panic: .asciz "Error: main() finished!\n"
panic: .asciz "BL panic!\n"
