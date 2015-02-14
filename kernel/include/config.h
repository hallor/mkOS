#ifndef CONFIG_H
#define CONFIG_H
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

// DDR
#define CONFIG_RAM_START 0x40000000
#define CONFIG_RAM_SIZE (0x8000000) // 128M

// "bios" qemu flash, also mapped to 0
#define CONFIG_NOR_CS0_START 0x00000000
#define CONFIG_NOR_CS0_SIZE  0x04000000 // 64M

#define CONFIG_NOR_CS1_START 0x04000000
#define CONFIG_NOR_CS1_SIZE  0x04000000 // 64M

// "system" qemu flash
//#define CONFIG_NOR_CS1_START 0x0c000000
//#define CONFIG_NOR_CS1_SIZE (64 * 1024 * 1024)

// pl011
#define CONFIG_UART0_ADDR 0x9000000

// sp804
//#define CONFIG_TIMER0_ADDR 0x1c110000

// pl031
#define CONFIG_RTC0_ADDR 0x09010000

#define CONFIG_MAX_FILE_NAME 32
#define CONFIG_MAX_FILES 64
#define CONFIG_PRINTK_BUFFER_LEN 1024

//default systimer frequency - 1s
#define CONFIG_TIMER_FREQ 1

#define CONFIG_GICD_CPU_BASE 0x08000000
#define CONFIG_GICC_CPU_BASE 0x08010000
#endif // CONFIG_H
