ENTRY(_start)

SECTIONS
{

    .init 0x80000 :
    {
        PROVIDE (__start = .);
        *(.init)
    }

    .text ALIGN(4096) :
    {
        *(.init)
        *(.text.unlikely .text.*_unlikely)
        *(.text.exit .text.exit.*)
        *(.text.startup text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
        *(.gnu.warning)
    }

    .rodata ALIGN(4096): 
    { 
        *(.data .data.* .gnu.linkonce.r*) 
    }

    .data ALIGN(4096):
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

    .bss ALIGN(4096) :
    {
        bss = .;
        __bss_start = .;
        *(COMMON)
        *(.bss .bss.*)
    }
    __bss_end = .;
    __bss_size = __bss_end - __bss_start;

    __kernel_end = .; 
}
