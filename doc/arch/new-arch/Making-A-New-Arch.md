Apollo Architectures : Creating a New one 
==========================================

Introduction
------------

So You've decided to create a new archetecture support for the Apollo kernel?
There is a few tasks to complete on your platform before Apollo will happily
run on top of it.

Prerequisites
-------------

In order for Apollo to work happily on your platform, it needs to have a few
items availible.

* Your platform has to be a 32 or 64 bit system. Apollo won't support 16 bit
    without massive core arch change.
* A Console device of some kind for boot messages, be it serial, a screen
    buffer, or even a line printer.
* Interrput capability. Without this, the system can't do syscalls from 
    usermode.
* A timer. The kernel uses this for scheduling, and timekeeping. High
    precision isn't needed, just at least 10KHz.

Boot time
---------

The Apollo kernel is a multiboot compliant kernel. The linker script tells
multiboot to look for a symbol called 'start'. 

The multiboot bootloader will be looking for a multiboot header, it's the
job of the boot.s file (or equivilent) to provide that. The flags used in the
Kernel should be:

* MULTIBOOT_MEMORY_INFO
* MULTIBOOT_PAGE_ALIGN

You can find more about the multiboot header from the multiboot specification.

It is your responsibility to write a function called start 

In your start function you should set up a stack, preferably at least 4k large.
It is also start's task to pass the multiboot header to main, stop interrupts,
and call main.

The stack should be aligned to a 32 byte boundary, in accordance to the SYSV
ABI.

Drivers To Implement
--------------------

When Apollo boots, it runs archInit() before doing kernel setup. archInit()
should initialize all of the drivers listed below to a known state.

* A [Console Driver](Making-A-Console-Driver.html). This is a character output
    device used to output the status of the kernel during bootup.

* An Interrupt Vector Driver. This driver should expose a function for creating
    interrupt functions for syscalls. The actual syscall interface is handled
    by the kernel.

* A Timer Driver. This should be a system timer that exposes functions to help
    with scheduling such as alarm() and getTicks().

* Memory handler Driver. This driver basically should give the kernel 4k pages
    when requested. This is used internally to power malloc().


