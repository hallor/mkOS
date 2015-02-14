include ../config.mk
ASFLAGS := $(ASFLAGS) -D__KERNEL__
CFLAGS := $(CFLAGS) -D__KERNEL__
LDFLAGS := $(LDFLAGS) -T boot.lds -Wl,--build-id=none
include ../build.mk

build: boot.elf boot.bin


objs:= init.o uart.o main.o
DEPS := $(addsuffix .d, $(objs))

clean:
	rm -f $(objs) $(DEPS) boot.elf

boot.bin: boot.elf
	$(OBJCOPY) $(OCFLAGS) -O binary $< $@

boot.elf : $(objs)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEPS)
