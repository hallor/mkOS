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
adr x0, msg
svc #0
b .
svc #1 // exit
// zero bss
adr x0, _sbss
adr x1, _ebss
sub x2, x0, x1
cbz x2, 2f
mov x2, #0
1:
	cmp x1, x0
	b.ge 2f
	str x2, [x1], #8
	b 1b

2:
	bl __libc_main
	svc #1 // exit task

1:
	wfe
	b 1b
msg: .asciz "Can't proceed further - need MMU :(\n"
