include ../config.mk
ASFLAGS := $(ASFLAGS) -D__KERNEL__
CFLAGS := $(CFLAGS) -D__KERNEL__
LDFLAGS := $(LDFLAGS) -T boot.lds
include ../build.mk

build: boot.elf


objs:= init.o uart.o main.o
DEPS := $(addsuffix .d, $(objs))

clean:
	rm -f $(objs) $(DEPS) kernel.elf

boot.elf : $(objs)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEPS)
