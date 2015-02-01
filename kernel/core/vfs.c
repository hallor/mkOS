#include "vfs.h"
#include "kmalloc.h"
#include "kern_console.h"
#include "u-boot-image.h"
#include "util.h"

struct inode {
    char name[CONFIG_MAX_FILE_NAME];
    void *blob_start; // @ flash
    unsigned blob_size;
    unsigned refcount;
};

static struct inode *vfs = 0;
static unsigned vfs_next = 0;

static void parse_flash(void * base, unsigned size)
{
    image_header_t * hdr = base;

    if (size < sizeof(image_header_t)) {
        puts("Suprious bytes at the end of device");
        return;
    }
    if (vfs_next == CONFIG_MAX_FILES) {
        puts("Too many files.\n");
        return;
    }

    size -= sizeof(image_header_t);
    if (ntohl(hdr->ih_magic) == IH_MAGIC) {
        putreg("Found image @", (unsigned)base);
        puts("image-name: "); puts((char*)hdr->ih_name); putreg(" image-size", ntohl(hdr->ih_size));
        if (hdr->ih_arch != IH_ARCH_ARM64) {
            puts("Invalid image architecture!\n");
            return;
        }
        if (hdr->ih_comp != IH_COMP_NONE) {
            puts("Image compression not supported!\n");
            return;
        }
        if (ntohl(hdr->ih_size) > size) {
            puts("Image too large!\n");
            return;
        }
        memcpy(vfs[vfs_next].name, hdr->ih_name, CONFIG_MAX_FILE_NAME); // TODO: strncpy
        vfs[vfs_next].blob_size = sizeof(image_header_t) + ntohl(hdr->ih_size);
        vfs[vfs_next].blob_start = base;
        vfs[vfs_next].refcount = 0;

        base += vfs[vfs_next].blob_size;
        size -= ntohl(hdr->ih_size);
        vfs_next++;

        parse_flash(base, size);
    } else {
        puts("No magic found - aborting.\n");
    }
}

struct file_descriptor {
    struct inode * file;
    unsigned offset;
    unsigned fd;
};

struct file_descriptor *fds = 0;
unsigned fds_next = 0;
unsigned fds_max = 100;

int vfs_init(void)
{
    puts("Initializing vfs...\n");
    vfs = kmalloc(CONFIG_MAX_FILES * sizeof(struct inode));
    if (!vfs) {
        puts("vfs init failed.\n");
        return -1;
    }
    vfs_next = 0;

    fds = kmalloc(sizeof(struct file_descriptor) * fds_max);
    if (!fds) {
        puts("fds init failed.\n");
        return -1;
    }
    fds_next = 0;
    memset(fds, 0, sizeof(struct file_descriptor) * fds_max);

    parse_flash((void*)CONFIG_NOR_CS0_START, CONFIG_NOR_CS0_SIZE);
    return 0;
}

static int create_fd(struct inode * node)
{
    if (fds_next >= fds_max)
        return -1;
    struct file_descriptor * fd = &fds[fds_next];
    fd->fd = fds_next;
    fd->file = node;
    fd->offset = 0;
    fd->file->refcount++;
    fds_next++;
    return fd->fd;
}

static struct file_descriptor * get_descriptor(int fd)
{
    if (fd > fds_max)
        return 0;
    if (fds[fd].file == 0)
        return 0;
    return &fds[fd];
}

static int delete_fd(int fd)
{
    struct file_descriptor * desc = get_descriptor(fd);
    if (!desc)
        return -1;
    desc->file->refcount--;
    desc->file = 0;
    desc->offset = 0;
    desc->fd = 0;
    return 0;
}

int vfs_open(const char * name)
{
    int i;
    for (i=0; i < vfs_next; ++i) {
        if (!strncmp(name, vfs[i].name, CONFIG_MAX_FILE_NAME)) {
            return create_fd(&vfs[i]);
        }
    }
    return -1;
}

int vfs_read(int fd, void * buf, unsigned size)
{
    struct file_descriptor * i = get_descriptor(fd);
    if (!i || !i->file)
        return -1;

    if (i->offset + size > i->file->blob_size)
        size = i->file->blob_size - i->offset;

    memcpy(buf, i->file->blob_start + i->offset, size);
    i->offset += size;
    putreg("vfs - bytes read", size);
    return size;
}

int vfs_write(int fd, void * buf, unsigned count)
{
    return -1;
}

int vfs_close(int fd)
{
    return delete_fd(fd);
}


int vfs_stat(int fd, struct stat *stat)
{
    struct file_descriptor * i = get_descriptor(fd);
    if (!i || !i->file || !stat)
        return -1;
    stat->size = i->file->blob_size;
    return 0;
}
