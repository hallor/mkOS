include ../config.mk
ASFLAGS := $(ASFLAGS) -Iinclude
CFLAGS := $(CFLAGS) -Iinclude -pie
LDFLAGS := $(LDFLAGS) -T app.lds -pie -static
include ../build.mk

build: test.img

clean:
	rm -f $(objs) $(DEPS) test.app test.bin test.img

subdirs := libc app1 ld

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
$(info Fix this makefile - it doesn't rebuild app properly.\n")

test.app.stripped: test.app
	cp -f $^ $@
	$(STRIP) -s $@

get_entry= $(shell readelf -h test.app  | grep "Entry point address:" | tr -s " " | cut -d " " -f 5)
test.img: test.bin test.app.stripped
	$(MKIMAGE) -A arm64 -O u-boot -T standalone -C none -a 0x80100000 -e $(get_entry) -n "test-app" -d test.app $@
