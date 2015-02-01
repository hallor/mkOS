#include "task.h"
#include "page.h"
#include "printk.h"
#include "util.h"
#include "vfs.h"
#include "u-boot-image.h"
#include "kmalloc.h"

#define MAX_TASKS 50
static struct task tasks[MAX_TASKS];

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
    task->vma_size = ntohl(hdr.ih_size);
    task->vma_addr = kmalloc(task->vma_size);
    task->ctx.gpr[0] = task->tid;
    task->ctx.lr = 0x0;
    task->ctx.sp = page_alloc(1); // one page for stack
    task->ctx.spsr = 0x3C0; // User, AA64
    task->ctx.pc = task->vma_addr + ntohl(hdr.ih_ep) - ntohl(hdr.ih_load);

    ret = vfs_read(fd, task->vma_addr, task->vma_size);
    if (ret != task->vma_size) {
        err("Failed to read executable. Task %s is invalid.\n", task->name);
        return;
    }

    task->state = TASK_ACTIVE;
    info("Created task '%s' with tid %d. VMA: %#llx pc = %#llx sp = %#llx\n",
         task->name, task->tid, task->vma_addr, task->ctx.pc, task->ctx.sp);
    vfs_close(fd);

    if (!current)
        current = task;
}

void task_create()
{
    int i;
    info("Creating tasks...\n");
    for (i=0; i<MAX_TASKS; ++i) {
        tasks[i].tid = i;
        tasks[i].state = TASK_INVALID;
    }
    for (i=0; i<3; ++i) {
        task_load("test-app");
    }
}
