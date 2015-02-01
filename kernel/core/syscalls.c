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

#include "task.h"
#include "printk.h"
typedef void (*sys_handler_t)(struct task *);


static void sys_print(struct task * task)
{
    dbg("(%s:%d) sys_print(%#llx)\n", task->name, task->tid, task->ctx.gpr[0]);
    puts((char*)task->ctx.gpr[0]);
}

static void sys_yield(struct task * task)
{
    dbg("(%s:%d) sys_yield()\n", task->name, task->tid);
    task_next();
}

static void sys_exit(struct task * task)
{
    dbg("(%s:%d) sys_exit()\n", task->name, task->tid);
    task->state = TASK_DEAD;
    task_next(); // switch to next task
}

sys_handler_t syscall_handlers[] = {
    sys_print,
    sys_exit,
    sys_yield,
};


void do_syscall(int no, struct task *task)
{
    if (no > (sizeof(syscall_handlers) / sizeof(sys_handler_t))) {
        err("Unknown syscall requested: %d.\n", no);
        return;
    }

    syscall_handlers[no](task);
}
