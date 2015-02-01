OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(start)

MEMORY
{
	ram(rwx): ORIGIN = 0x80000000, LENGTH = 64M
}

ram_top = ORIGIN(ram) + LENGTH(ram);

SECTIONS
{
	. = ALIGN(16);
	.text : {
		_stext = .;
		*(.vectors)
		*(.text)
		*(.rodata)                 /* read-only data (constants) */
		*(.rodata*)
		. = ALIGN(16);
		_etext = . ;
	} > ram

	/* collect all initialized .data sections */
	.data : {
		_sdata = .;
		*(.vectors)
		*(.data)
		. = ALIGN(16);
		_edata = .;
	} > ram

	/* collect all uninitialized .bss sections */
	.bss (NOLOAD) : {
		. = ALIGN(16);
		_sbss = .;
		*(.bss*)
		. = ALIGN(16);
		_ebss = .;
	} > ram
	. += 16384;
	. = ALIGN(4096);
	_stack_top = .;
	_heap_start = .;
}
end = .;  /* define a global symbol marking the end of application */
