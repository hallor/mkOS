obj-y += basic.o
TGT:=basic

CROSS_COMPILE:=aarch64-linux-gnu-
AS       := $(CROSS_COMPILE)as
LD       := $(CROSS_COMPILE)ld
CC       := $(CROSS_COMPILE)gcc
CPP      := $(CROSS_COMPILE)gcc -E
CXX      := $(CROSS_COMPILE)g++
AR       := $(CROSS_COMPILE)ar
RANLIB   := $(CROSS_COMPILE)ranlib
NM       := $(CROSS_COMPILE)nm
STRIP    := $(CROSS_COMPILE)strip
OBJCOPY  := $(CROSS_COMPILE)objcopy
OBJDUMP  := $(CROSS_COMPILE)objdump


CFLAGS := -Wall -g -ggdb -nostdinc -fno-builtin
CPPFLAGS := $(CFLAGS)
LDFLAGS := -nostdlib -nostartfiles -fno-builtin -T image.lds
LIBS :=  -g -ggdb
ASFLAGS := -D__ASSEMBLY__
DEPFLAGS = -MMD -MF $(@F).d

OBJS:=$(obj-y)

DEPS := $(addsuffix .d, $(OBJS))

all: $(TGT)

$(TGT): $(OBJS) image.lds
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TGT)

%.o: %.c Makefile
	$(CC) $(DEPFLAGS) $(CFLAGS) -o $@ -c $<

%.o: %.cpp Makefile
	$(CXX) $(DEPFLAGS) $(CPPFLAGS) -o $@ -c $<

%.o: %.S Makefile
	$(CC) $(DEPFLAGS) $(ASFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJS) $(DEPS) $(TGT)

-include $(DEPS)

