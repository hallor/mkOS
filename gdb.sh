#!/bin/sh
aarch64-linux-gnu-gdb \
-ex "add-symbol-file boot/boot.elf 0x42000000" \
-ex "add-symbol-file boot/kernel.elf 0x40000000" \
-ex "target remote localhost:1234" \
-ex "info registers"
