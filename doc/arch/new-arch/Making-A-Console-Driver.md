Creating a Console Driver
==========================

Introduction
-------------

One of the essential parts of the boot process is a console terminal driver.
This driver allows the kernel to output early debug information before the
system comes online. The console driver is part of the kernel. The kernel
defines console_write(), which allows a string to be outputted to console.

Inside the architecture folder itself, you must implement a function called
void console_put(char). The Console driver will call this function to output
a single charecter to the console device.

Initialization
---------------

The kernel will call archInit() before it attempts to use the Console driver.
See [Making a New Architecture](Making-A-New-Arch.html) for more information 
about archInit().

Control Charecters
-------------------

console_put(char) accepts an ASCII charecter as input, but it should also 
support the following basic ascii control characters:

\t or 0x09 : Tab. This should move the cursor to the next tab indent.
             We recommend using 8 space tabs, though this is left up to 
             the implementation.

\n or 0x0A : Newline. This should drop to a new line, and also perform a 
             carriage return back to Column 0.

\f or 0x0C : Form feed: This should either clear the screen, or on a non-screen
             console, create some sort of page break like a horizontal rule
             does in html.

