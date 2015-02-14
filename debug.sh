#!/bin/sh
qemu-system-aarch64 -s -S -cpu cortex-a57 -machine virt -nographic \
-drive file=boot.cfi,if=pflash,unit=0,format=raw -drive file=flash.cfi,if=pflash,unit=1,format=raw || tset
