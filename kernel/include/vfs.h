#ifndef VFS_H
#define VFS_H
#include <stdint.h>
#include "config.h"


struct fops {
    int (*read)(void * d, unsigned off, unsigned size);
    int (*write)(void * d, unsigned off, unsigned size);
};

struct inode {
    unsigned number;
    struct fops ops;
};

struct file {
    char name[CONFIG_MAX_FILE_NAME];
    struct inode node;
};

int vfs_init(void);
struct file * vfs_get_file(const char * name);

#endif // VFS_H
