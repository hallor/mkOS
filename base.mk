#export ROOT := /home/hallor/Projects/aarch64

define add_subdir
$(1): $(1)-build
$(1)-build:
	make -C $(1)
$(1)-clean:
	make -C $(1) clean

build: $(1)-build
clean: $(1)-clean
endef
