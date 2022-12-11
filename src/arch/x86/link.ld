/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * Entry point for the rpi4 architecture port
 */

OUTPUT_ARCH("i386")
OUTPUT_FORMAT("elf32-i386")

/* Higher half on 32 bit addressing */
VIRT_OFFSET = 0xC0000000;

/* Multiboot compliance dumps us here */
INIT_OFFSET = 0x100000;

/* Defined in multiboot.s */
ENTRY(_start)

/* Defines the actual sections */
INCLUDE scripts/common.ld
