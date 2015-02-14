#all: kernel.img
all: build flash.cfi boot.cfi
clean:

subdirs := boot kernel userspace
#userspace kernel

include base.mk
#include config.mk

$(foreach dir,$(subdirs),$(eval $(call add_subdir,$(dir))))

#LDFLAGS := -nostdlib -nostartfiles -fno-builtin -T image.lds -mcpu=cortex-a57 -O0 -fPIC

#kernel.img: userspace-build kernel-build
	#$(CC) $(LDFLAGS) kernel/kernel.a userspace/test.o -o $@

kernel/kernel.img: kernel-build
userspace/test.img: userspace-build
boot/boot.bin: boot-build


boot.img: boot/boot.bin
	cat $^ > $@

boot.cfi: boot.img
	cp -f $^ $@
	truncate -s 64M $@

flash.img: kernel/kernel.img userspace/test.img
	cat $^ > $@

flash.cfi: flash.img
	cp -f $^ $@
	truncate -s 64M $@

clean:
	rm -f flash.cfi flash.img boot.img boot.cfi
