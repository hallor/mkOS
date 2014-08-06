#!/bin/sh
qemu-system-aarch64 -machine vexpress-a15 -cpu cortex-a57 -kernel kernel/kernel.elf -m 64M -nographic -bios flash.cfi || tset
