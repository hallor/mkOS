include ../config.mk
ASFLAGS := $(ASFLAGS) -Iinclude
CFLAGS := $(CFLAGS) -Iinclude
LDFLAGS := $(LDFLAGS) -T app.lds
include ../build.mk

build: test.bin

clean:
	rm -f $(objs) $(DEPS) test.app test.bin

subdirs := libc app1

define add_files
obj-y:=
$(eval include $(1)/Makefile)
$(eval objs+= $(addprefix $(1)/,$(obj-y)))
obj-y:=
endef

$(foreach dir,$(subdirs),$(eval $(call add_files,$(dir))))

test.app: $(objs)
	$(CC) $(LDFLAGS) -Wl,--start-group $^ -Wl,--end-group -o $@

test.bin: test.app
	$(OBJCOPY) -O binary test.app test.bin

test.o: test.bin
	$(OBJCOPY) -I binary -O elf64-little --binary-architecture aarch64 $^ $@
