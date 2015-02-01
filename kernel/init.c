#include <stdint.h>
#include "kern_console.h"
#include "task.h"
#include "vfs.h"
#include "page.h"
#include "util.h"
#include "kmalloc.h"
#include "printk.h"

/* Memory map:
 * VE_NORFLASH1
 *VE_NORFLASHALIAS -> 0x0
 *64M
 *0x08000000 -> norflash0 cfi -> bios
 *0x0c000000 -> norflash1 cfi
 *    [VE_UART0] = 0x1c090000, pl011
    [VE_TIMER01] = 0x1c110000, sp804
    [VE_RTC] = 0x1c170000, <- pl031
    [VE_COMPACTFLASH] = 0x1c1a0000, N/A

 **/

void init_kernel(void)
{
    info("Initializing kernel...\n");
    page_init();
    kmalloc_init();
    vfs_init();
    task_create();
}

