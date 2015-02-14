#!/bin/sh
qemu-system-aarch64 -s -machine virt -cpu cortex-a57 -kernel boot/boot.elf -m 64M -nographic -bios flash.cfi || tset
