ENTRY(start)
SECTIONS
{
	.text 0x100000 :
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(4096);
	}

	.data :
	{
        PROVIDE (__startup_begin = .);
        *(.startup)
        PROVIDE (__startup_end = .);
        PROVIDE (__shutdown_begin = .);
        *(.shutdown)
        PROVIDE (__shutdown_end = .);

        data = .; _data = .; __data = .;
		*(.data .data.* .gnu.linkonce.d)
        PROVIDE(__ctors_begin = .);
        *(.ctors)
        PROVIDE(__ctors_end = .);
		*(.rodata)
		. = ALIGN(4096);
	}

	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss .bss.* .gnu.linkonce.b.*)
		. = ALIGN(4096);
	}
    PROVIDE(__end = .);
	end = .; _end = .; __end = .;
    
}
