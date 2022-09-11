Creating a Console Driver
==========================

Introduction
-------------
One of the essential parts of the boot process is a console terminal driver.
This driver allows the kernel to output early debug information before the
system comes online. The console driver is part of the kernel. The kernel
defines access to logging functions but it is up to the architecture to create
a console driver and initialize it.

Initialization
---------------
Calling `register_console()` is how you register a new console with the kernel.
The only functions required to implement are `init()` and `write()`.

Control Charecters
-------------------
The kernel internally accepts ascii data for logging, but it also uses several
ASCII control codes which should be implemented by your console:

\t  Tab.    This should move the cursor to the next tab indent.
            We recommend using 8 space tabs, though this is left up to 
            the implementation.

\n  Newline. This should drop to a new line, and also perform a 
            carriage return back to Column 0.

\f  Form feed. This should either clear the screen, or on a non-screen
            console, create some sort of page break like a horizontal rule
            does in html.

\b  Backspace. This should move the cursor backwards one character but not null
               out the character glyph at that location.

Color Codes
-----------
Parts of the kernel use ANSI escape codes for colors. If your console supports
color codes, try to support them. If your console does not, these codes may need
to be stripped out of the data stream.
