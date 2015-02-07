#ifndef TASK_H
#define TASK_H
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
#include "config.h"
#include "cpu.h"

enum task_state {
    TASK_INVALID = 0,
    TASK_ACTIVE,
    TASK_DEAD,
};

struct task {
    struct cpu_ctx ctx; // must be first (for now)
    char name[CONFIG_MAX_FILE_NAME];
    unsigned tid;
    enum task_state state;
    void * vma_addr;
    unsigned vma_size;
    void * stack_base;
    unsigned stack_size;
};

extern struct task * current;

void task_create(void);
void task_next(void);
void task_exit(struct task * task);
#endif // TASK_H
