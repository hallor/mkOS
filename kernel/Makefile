include ../config.mk
ASFLAGS := $(ASFLAGS) -I$(CURDIR)/include -D__KERNEL__
CFLAGS := $(CFLAGS) -I$(CURDIR)/include -D__KERNEL__
LDFLAGS := $(LDFLAGS) -T kernel.lds
include ../build.mk

build: kernel.img

subdirs := core drivers lib lib_3rdparty
objs:= entry.o exception.o init.o

DEPS := $(addsuffix .d, $(objs))
define add_files
obj-y:=
$(eval include $(1)/Makefile)
$(eval objs+= $(addprefix $(1)/,$(obj-y)))
$(eval DEPS+= $(addprefix $(1)/,$(addsuffix .d,$(obj-y))))
obj-y:=
endef
# CFLAGS += -I$(1)

$(foreach dir,$(subdirs),$(eval $(call add_files,$(dir))))

clean:
	rm -f $(objs) $(DEPS) kernel.elf kernel.img kernel.bin

get_entry= $(shell readelf -h kernel.elf  | grep "Entry point address:" | tr -s " " | cut -d " " -f 5)
kernel.img: kernel.bin kernel.elf
	$(MKIMAGE) -A arm64 -O u-boot -T kernel -C none -a 0x40000000 -e $(get_entry) -n "mkOS-kernel" -d kernel.bin $@

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@

kernel.elf : $(objs)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEPS)
