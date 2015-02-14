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

#include <stdint.h>
#include "kern_console.h"
#include "task.h"
#include "vfs.h"
#include "page.h"
#include "util.h"
#include "kmalloc.h"
#include "printk.h"
#include "timer.h"
#include "gic.h"

/* Memory map:
 * VE_NORFLASH1
 *VE_NORFLASHALIAS -> 0x0
 *64M
 *0x08000000 -> norflash0 cfi -> bios
 *0x0c000000 -> norflash1 cfi
 *    [VE_UART0] = 0x1c090000, pl011
    [VE_TIMER01] = 0x1c110000, sp804
    [VE_RTC] = 0x1c170000, <- pl031
    [VE_COMPACTFLASH] = 0x1c1a0000, N/A

 **/

void init_kernel(void)
{
    info("Initializing kernel...\n");
    page_init();
    kmalloc_init();
    vfs_init();
    gic_init();
    timer_init();
    panic("bleble");
    task_create();
}

