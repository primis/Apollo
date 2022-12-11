/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE 
 * Common link file (Things that are shared across all architectures)
 */

SECTIONS
{
    . = INIT_OFFSET;
    
    .init :
    {
        PROVIDE (__start = .);
        *(.init)
    } : init

    .init.bss ALIGN(4096) :
    {
        *(.init.bss)
    } : init.bss
    /* Higher half stuff, add 3GB to all addresses below */
    . += VIRT_OFFSET;

    .text ALIGN(4096) : AT(ADDR(.text) - VIRT_OFFSET)
	{
        *(.mboot)
        *(.text .stub .text.* .gnu.linkonce.t.*)
	} : text

    .rodata ALIGN(4096) : AT(ADDR(.rodata) - VIRT_OFFSET)
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    } : rodata

	.data ALIGN(4096) : AT(ADDR(.data) - VIRT_OFFSET)
	{
        *(.data .data.* .gnu.linkonce.d)
    } : data

    .structured ALIGN(4096) : AT(ADDR(.structured) - VIRT_OFFSET)
    {
        PROVIDE( __modules_begin = .);
        *(.modules)
        PROVIDE (__modules_end = .);
        PROVIDE(__drivers_begin = .);
        *(.drivers)
        PROVIDE(__drivers_end = .);
	} : data

	.bss ALIGN(4096) : AT(ADDR(.bss) - VIRT_OFFSET) 
	{
        PROVIDE(__bss_begin = . - VIRT_OFFSET);
		*(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        PROVIDE(__bss_end = . - VIRT_OFFSET);
        PROVIDE(__end = . - VIRT_OFFSET);
    } : data
    
    /* end of kernel symbol */
    __kernel_end = .; 
}

PHDRS
{
	init PT_LOAD FLAGS(5);
	init.bss PT_LOAD FLAGS(6);
    text PT_LOAD FLAGS(5);
	rodata PT_LOAD FLAGS(4);
	data PT_LOAD FLAGS(6);
}
