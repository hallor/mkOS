include ../config.mk
ASFLAGS := $(ASFLAGS) -Iinclude -D__KERNEL__
CFLAGS := $(CFLAGS) -Iinclude -D__KERNEL__
LDFLAGS := $(LDFLAGS) -T kernel.lds
include ../build.mk

build: kernel.elf


subdirs := drivers
objs:= el1_entry.o el1_main.o el1_vector.o kern_console.o syscalls.o

define add_files
obj-y:=
$(eval include $(1)/Makefile)
$(eval objs+= $(addprefix $(1)/,$(obj-y)))
CFLAGS += -I$(1)
obj-y:=
endef
DEPS := $(addsuffix .d, $(objs))

$(foreach dir,$(subdirs),$(eval $(call add_files,$(dir))))

clean:
	rm -f $(objs) $(DEPS) kernel.elf

kernel.elf : $(objs)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEPS)
