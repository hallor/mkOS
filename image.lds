OUTPUT_FORMAT("elf64-littlearm", "elf64-littlearm", "elf64-littlearm")
OUTPUT_ARCH(arm)
ENTRY(reset)
SECTIONS
{
	. = ALIGN(4);
	.text : {
		_stext = .;
		*(.text)
		*(.rodata)                 /* read-only data (constants) */
		*(.rodata*)
		. = ALIGN(16);
		_etext = . ;
	}

	/* collect all initialized .data sections */
	.data : {
		_sdata = .;
		*(.vectors)
		*(.data)
		_edata = .;
	}

	/* collect all uninitialized .bss sections */
	.bss (NOLOAD) : {
		. = ALIGN(16);
		_sbss = .;
		*(.bss)
		_ebss = .;
	}
}
end = .;  /* define a global symbol marking the end of application */
