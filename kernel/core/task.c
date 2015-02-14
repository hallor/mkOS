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
#include "page.h"
#include "printk.h"
#include "util.h"
#include "vfs.h"
#include "u-boot-image.h"
#include "kmalloc.h"

#define MAX_TASKS 50
static struct task tasks[MAX_TASKS];
static struct task idle;

struct task *current = 0;

void task_next()
{
    int i;

    for (i=current->tid + 1; i<current->tid + 1 + MAX_TASKS; ++i) {
        if (tasks[i % MAX_TASKS].state == TASK_ACTIVE) {
            current = &tasks[i % MAX_TASKS];
            break;
        }
    }

    if (current->state != TASK_ACTIVE) {
        panic("No more tasks are active.\n");
        current = &idle;
    }
    dbg("context switch to %s tid %d\n", current->name, current->tid);
}

struct task * find_free_task()
{
    int i;
    for (i=0; i<MAX_TASKS; ++i) {
        if (tasks[i].state == TASK_INVALID) {
            return &tasks[i];
        }
    }
    return 0;
}

int load_elf(struct task * task, void * blob, unsigned size);
static void task_load(const char *name)
{
    int fd = vfs_open(name);
    dbg("Loading task: %s\n", name);
    if (fd < 0) {
        err("Failed to open task %s\n", name);
        return;
    }

    image_header_t hdr;
    int ret = vfs_read(fd, &hdr, sizeof(image_header_t));

    if (ret != sizeof(image_header_t)) {
        err("Failed to read task header\n");
        vfs_close(fd);
        return;
    }


    struct task * task = find_free_task();
    if (!task) {
        err("No more free tasks\n");
        vfs_close(fd);
        return;
    }

    memcpy(task->name, hdr.ih_name, CONFIG_MAX_FILE_NAME); // todo: strncpy
    task->vma_size = 0;
    task->vma_addr = 0;
    task->stack_base = page_alloc(2);
    task->stack_size = PAGE_SIZE * 2;
    task->ctx.gpr[0] = task->tid;
    task->ctx.lr = 0x0;
    task->ctx.sp = task->stack_size; // two pages for stack
    task->ctx.spsr = 0x300; // User, AA64, enabled interrupts
    task->ctx.pc = 0;

    // for now use page allocator as it's able to free memory contrary to kmalloc
    unsigned data_size = ntohl(hdr.ih_size);
    void * data_blob = page_alloc(ALIGN_PGUP(data_size) >> PAGE_SHIFT);
    ret = vfs_read(fd, data_blob, data_size);
    if (ret != ntohl(hdr.ih_size)) {
        err("Failed to read executable. Task %s is invalid.\n", task->name);
        return;
    }
    ret = load_elf(task, data_blob, data_size);
    page_free(data_blob, ALIGN_PGUP(data_size) >> PAGE_SHIFT);
    if (ret) {
        err("Failed to parse elf.\n");
        return;
    }
    task->ctx.pc = (uintptr_t)task->vma_addr + ntohl(hdr.ih_ep) - ntohl(hdr.ih_load);

    task->state = TASK_ACTIVE;
    info("Created task '%s' with tid %d. VMA: %#llx pc = %#llx sp = %#llx\n",
         task->name, task->tid, task->vma_addr, task->ctx.pc, task->ctx.sp);
    vfs_close(fd);

    if (!current)
        current = task;
}

void task_exit(struct task * task)
{
    info("task_exit task %s tid %d\n", task->name, task->tid);
    page_free(task->stack_base, task->stack_size >> PAGE_SHIFT);
    page_free(task->vma_addr, task->vma_size >> PAGE_SHIFT);
    task->state = TASK_INVALID;
}

static void idle_loop(void)
{
    asm("wfi");
}

static void add_idle(void)
{
    struct task * task = &idle;
    memcpy(task->name, "idle", 5); // todo: strncpy
    task->vma_size = 0;
    task->vma_addr = 0;
    task->stack_base = page_alloc(1);
    task->stack_size = PAGE_SIZE * 2;
    task->ctx.gpr[0] = 0;
    task->ctx.lr = 0x0;
    task->ctx.sp = task->stack_size; // two pages for stack
    task->ctx.spsr = 0x300; // User, AA64, enabled interrupts
    task->ctx.pc = (uint64_t)idle_loop;
    task->state = TASK_ACTIVE;
}

void task_create()
{
    int i;
    info("Creating tasks...\n");
    for (i=0; i<MAX_TASKS; ++i) {
        tasks[i].tid = i;
        tasks[i].state = TASK_INVALID;
    }
    info("Creating idle task...\n");
    add_idle();
    for (i=0; i<2; ++i) {
        task_load("test-app");
    }
}
