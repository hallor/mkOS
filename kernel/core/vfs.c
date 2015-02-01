#include "vfs.h"
#include "kern_console.h"

static int file_read(void * d, unsigned off, unsigned size)
{
    return -1;
}

static int file_write(void * d, unsigned off, unsigned size)
{
    return -1;
}

static struct file files[CONFIG_MAX_FILES];

int vfs_init(void)
{
    puts("Initializing vfs...\n");
    return 0;
}

struct file * vfs_get_file(const char * name)
{
    return 0;
}
