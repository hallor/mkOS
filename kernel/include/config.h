#ifndef CONFIG_H
#define CONFIG_H

// DDR
#define CONFIG_RAM_START 0x80000000
#define CONFIG_RAM_SIZE (64 * 1024 * 1024)

// "bios" qemu flash, also mapped to 0
#define CONFIG_NOR_CS0_START 0x08000000
#define CONFIG_NOR_CS0_SIZE (64 * 1024 * 1024)

// "system" qemu flash
#define CONFIG_NOR_CS1_START 0x0c000000
#define CONFIG_NOR_CS1_SIZE (64 * 1024 * 1024)

// pl011
#define CONFIG_UART0_ADDR 0x1c090000

// sp804
#define CONFIG_TIMER0_ADDR 0x1c110000

// pl031
#define CONFIG_RTC0_ADDR 0x1c170000


#define CONFIG_MAX_FILE_NAME 32
#define CONFIG_MAX_FILES 64
#endif // CONFIG_H
