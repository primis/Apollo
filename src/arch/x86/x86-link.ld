OUTPUT_ARCH("i386")
OUTPUT_FORMAT("elf32-i386")

ENTRY(_start)

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
    /* Higher half stuff, add 3GB to all addresses below */
    . += 0xC0000000;

    .text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000)
	{
        *(.mboot)
        *(.text.unlikely .text.*_unlikely)
        *(.text.exit .text.exit.*)
        *(.text.startup text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
        *(.gnu.warning)
	}
    
    .rodata ALIGN(4096) : AT(ADDR(.rodata) - 0xC0000000) 
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

	.data ALIGN(4096) : AT(ADDR(.data) - 0xC0000000)
	{
        PROVIDE (__startup_begin = .);
        *(.startup)
        PROVIDE (__startup_end = .);
        PROVIDE (__shutdown_begin = .);
        *(.shutdown)
        PROVIDE (__shutdown_end = .);
		
        *(.data .data.* .gnu.linkonce.d)
        SORT(CONSTRUCTORS)

        PROVIDE(__ctors_begin = .);
        *(.ctors)
        PROVIDE(__ctors_end = .);
	}

	.bss ALIGN(4096) : AT(ADDR(.bss) - 0xC0000000) 
	{
		*(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        PROVIDE(__end = . - 0xC0000000);
	}
    /* end of kernel symbol */
    __kernel_end = .; 
}
