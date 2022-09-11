Apollo Architectures : Creating a New one 
==========================================

Introduction
------------

So You've decided to create a new architecture support for the Apollo kernel?
There is a few tasks to complete on your platform before Apollo will happily
run on top of it.

Prerequisites
-------------

In order for Apollo to work happily on your platform, it needs to have a few
items availible.

* Your platform has to be a 32 or 64 bit system. Apollo won't support 16 bit
    without massive core arch change.
* A console device of some kind for boot messages, be it serial, a screen
    buffer, or even a line printer.
* Interrput capability. Without this, the system can't do syscalls from 
    usermode.
* A timer. The kernel uses this for scheduling, and timekeeping. High
    precision isn't needed, just at least 1KHz.

Boot time
---------
It is a requirement that you set up your system into a well known state.
In your start function you should set up a stack, preferably at least 4k large.
It is also start's task to set up the stack, stop interrupts, and call main.

The stack should be aligned to a 32 byte boundary, in accordance to the SYSV
ABI.

Drivers To Implement
--------------------

After main has been called, Apollo enumerates all modules compiled within it and
runs their init functions. The bare minimum a platform should support is:

* A [Console Module](Making-A-Console-Driver.html), implementing a console 
    driver. This device used to output the status of the kernel during bootup.

* An interrupt vector module. This driver should expose a function for creating
    interrupt functions for syscalls. The actual syscall interface is handled
    by the kernel.

* A time keeping module. This should be a system timer that exposes functions to
    help with scheduling such as alarm() and getTicks().

* Memory handler driver. This driver basically should give the kernel 4k pages
    when requested. This is used internally to power malloc().
