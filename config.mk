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


ASFLAGS := -D__ASSEMBLY__ -mcpu=cortex-a57
CFLAGS := -Wall -g -ggdb -fno-builtin -mcpu=cortex-a57 -O0 -fPIC
LDFLAGS := -nostdlib -nostartfiles -fno-builtin -mcpu=cortex-a57 -O0 -fPIC -g -ggdb
DEPFLAGS = -MMD -MF $(@F).d
