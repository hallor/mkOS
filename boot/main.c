#include <stdint.h>
#include "config.h"
#include "image.h"

int uart_get(void);
void uart_put(int c);
void uart_puts (char*);

void puthex(uint64_t n)
{
    int zeros = 1;
    int c;
    int i;
    uart_puts("0x");
    for (i=15; i>=0; i--) {
        c = (n >> (4 * i)) & 0xF;
        if (c != 0)
            zeros = 0;
        if (zeros)
            continue;
        if (c < 10)
            uart_put('0' + c);
        else
            uart_put('A' + (c - 10));
    }
    if (zeros)
        uart_put('0');
}

void memcpy(void * dest, const void * src, unsigned cnt)
{
    while (cnt--) {
        *((char*)dest) = *((char*)src);
        dest++;
        src++;
    }
}

uint32_t ntohl(uint32_t d)
{
    return  ((d & 0x000000ffUL) << 24) |
            ((d & 0x0000ff00UL) <<  8) |
            ((d & 0x00ff0000UL) >>  8) |
            ((d & 0xff000000UL) >> 24);
}

#define dbg(x) do { uart_puts("[D] "); uart_puts(x); } while (0)
#define print(x) uart_puts(x)

void load_kernel(void * from)
{
    const image_header_t * image = from;
    print("Loading kernel from "); puthex((uint64_t)from); print("\n");
    if (ntohl(image->ih_magic) != IH_MAGIC) {
        print("Invalid image magic! Expected: "); puthex(IH_MAGIC); print(" Got: "); puthex(image->ih_magic); print("\n");
        return;
    }
    if (image->ih_arch != IH_ARCH_ARM64) {
        print("Invalid image architecture!\n");
        return;
    }
    if (image->ih_os != IH_OS_FIRMWARE) {
        print("Invalid image OS type!\n");
        return;
    }
    if (image->ih_comp != IH_COMP_NONE) {
        print("Image compression not supported!\n");
        return;
    }
    if (image->ih_type != IH_TYPE_KERNEL) {
        print("Invalid image type\n");
        return;
    }
    if (ntohl(image->ih_load) < CONFIG_RAM_START || ntohl(image->ih_load) >= CONFIG_RAM_START + CONFIG_RAM_SIZE/2 ) {
        print("Image doesn't fit in RAM]n");
        return;
    }
    if (ntohl(image->ih_load) + ntohl(image->ih_size) > CONFIG_RAM_START + CONFIG_RAM_SIZE/2) {
        print("Image too large\n");
        return;
    }
    print("Loading image: "); print((char*)image->ih_name); print("\n");
    print("To: "); puthex(ntohl(image->ih_load)); print(" Size: "); puthex(ntohl(image->ih_size)); print("\n");
    memcpy((void*)ntohl(image->ih_load), image + 1, ntohl(image->ih_size));
    print("Running image at "); puthex(ntohl(image->ih_ep)); print("\n");
    asm ("blr %0\n" : : "r"(ntohl(image->ih_ep)));
    print("Panic: image returned to bootloader.\n");
}

int main(void)
{
    dbg("Now running in C\n");
    load_kernel((void*)CONFIG_NOR_CS0_START);
    while(1) {}
}
