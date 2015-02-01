OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(start)

MEMORY
{
    ram(rwx): ORIGIN = 0x80000000 + 33554432, LENGTH = 33554432
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
	. = ALIGN(4096);
	_heap_start = .;
	. += 4096 ;
	_user_stack_top = . ;

	_stack_top = ram_top - 16;
}
end = .;  /* define a global symbol marking the end of application */
