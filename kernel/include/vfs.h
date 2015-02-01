#ifndef VFS_H
#define VFS_H
#include <stdint.h>
#include "config.h"

struct stat {
    unsigned size;
};

int vfs_init(void);
int vfs_open(const char * name);
int vfs_read(int fd, void * buf, unsigned size);
int vfs_write(int fd, void * buf, unsigned count);
int vfs_close(int fd);
int vfs_stat(int fd, struct stat * stat);
#endif // VFS_H
