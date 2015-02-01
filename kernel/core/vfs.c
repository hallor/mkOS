#include "vfs.h"
#include "kmalloc.h"
#include "kern_console.h"
#include "u-boot-image.h"
#include "util.h"

struct file_int {
    struct file f;
    void * blob_start; // @ flash
    unsigned blob_size;
};

static struct file_int *files = 0;
static unsigned free_entry = 0;

static int file_read(struct file_int * file, void * d, unsigned off, unsigned size)
{
    return -1;
}

static int file_write(void * d, unsigned off, unsigned size)
{
    return -1;
}


static void parse_flash(void * base, unsigned size)
{
    image_header_t * hdr = base;

    if (size < sizeof(image_header_t)) {
        puts("Suprious bytes at the end of device");
        return;
    }
    if (free_entry == CONFIG_MAX_FILES) {
        puts("Too many files.\n");
        return;
    }

    size -= sizeof(image_header_t);
    if (ntohl(hdr->ih_magic) == IH_MAGIC) {
        putreg("Found image @", base);
        puts("image-name: "); puts(hdr->ih_name); putreg(" image-size", ntohl(hdr->ih_size));
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
        files[free_entry].blob_size = sizeof(image_header_t) + ntohl(hdr->ih_size);
        files[free_entry].blob_start = base;
        memcpy(files[free_entry].f.name, hdr->ih_name, CONFIG_MAX_FILE_NAME); // TODO: strncpy
        base += files[free_entry].blob_size;
        size -= ntohl(hdr->ih_size);
        free_entry++;
        parse_flash(base, size);
    } else {
        puts("No magic found - aborting.\n");
    }
}

int vfs_init(void)
{
    int i;
    puts("Initializing vfs...\n");
    files = kmalloc(CONFIG_MAX_FILES * sizeof(struct file));
    if (!files)
        puts("vfs init failed.\n");
    for (i=0; i<CONFIG_MAX_FILES; ++i)
        files[i].blob_size = 0;

    parse_flash((void*)CONFIG_NOR_CS0_START, CONFIG_NOR_CS0_SIZE);
    return 0;
}

struct file * vfs_get_file(const char * name)
{
    return 0;
}
