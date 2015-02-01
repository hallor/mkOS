include ../config.mk
ASFLAGS := $(ASFLAGS) -I$(CURDIR)/include -D__KERNEL__
CFLAGS := $(CFLAGS) -I$(CURDIR)/include -D__KERNEL__
LDFLAGS := $(LDFLAGS) -T kernel.lds
include ../build.mk

build: kernel.elf


subdirs := core drivers lib
objs:= el1_entry.o el1_main.o el1_vector.o

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
	rm -f $(objs) $(DEPS) kernel.elf

kernel.elf : $(objs)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(DEPS)
