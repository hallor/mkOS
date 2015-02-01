#ifndef VFS_H
#define VFS_H
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
