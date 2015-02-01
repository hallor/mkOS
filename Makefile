#all: kernel.img
all: build flash.cfi
clean:

subdirs := boot kernel userspace
#userspace kernel

include base.mk
#include config.mk

$(foreach dir,$(subdirs),$(eval $(call add_subdir,$(dir))))

#LDFLAGS := -nostdlib -nostartfiles -fno-builtin -T image.lds -mcpu=cortex-a57 -O0 -fPIC

#kernel.img: userspace-build kernel-build
	#$(CC) $(LDFLAGS) kernel/kernel.a userspace/test.o -o $@


flash.img: userspace-build
	cp -f userspace/test.bin flash.img

flash.cfi: flash.img
	cp -f $^ $@
	truncate -s 64M $@

clean:
	rm -f flash.cfi flash.img
