ENTRY(start)
SECTIONS
{
    .init 0x100000 :
    {
        PROVIDE (__start = .);
        *(.init)
    }
    
    .init.bss ALIGN(4096) :
    {
        *(.init.bss)
    }

    .text ALIGN(4096) :
	{
		code = .; _code = .; __code = .;
		*(.text)
        *(.text.exit .text.exit.*)
        *(.text.startup text.startup.*)
	}

	.data ALIGN(4096) :
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
	}

	.bss ALIGN(4096):
	{
		bss = .; _bss = .; __bss = .;
		*(.bss .bss.* .gnu.linkonce.b.*)
	}
    PROVIDE(__end = .);
	end = .; _end = .; __end = .;
    
}
