/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * x86 architecture specific linker script
 */

/* TODO: Higher half for 64-bit addressing */
VIRT_OFFSET = 0x0;

/* Multiboot compliance dumps us here */
INIT_OFFSET = 0x80000;

/* Defined in boot.S */
ENTRY(_start)

/* Defines the actual sections */
INCLUDE scripts/common.ld
