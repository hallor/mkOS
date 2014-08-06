#!/bin/sh
echo "Attach gdb"
qemu-system-aarch64 -machine vexpress-a15 -cpu cortex-a57 -kernel kernel/kernel.elf -m 64M -s -S -nographic -bios flash.cfi || tset
