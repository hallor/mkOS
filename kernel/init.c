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

void timer_do(void);
void gic_do(void);
void init_kernel(void)
{
    info("Initializing kernel...\n");
    page_init();
    kmalloc_init();
    vfs_init();
    gic_init();
    timer_init();
    gic_enable_interrupts();
    task_create();
}

