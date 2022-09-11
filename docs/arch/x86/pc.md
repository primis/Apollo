x86 PC
======

Introduction
------------

The IBM compatible PC has been around since the 80's. In respect to the Apollo
kernel, this architecture specifically refers to 32 bit processors. Apollo
currently targets i686+ processors. This places a lower bound on machines to 
around 1995 with the Pentium Pro.

Standard Porting Base
---------------------
Due to the ubiquity of this platform, it has been determined that the Apollo
kernel shall be developed with it as it's porting base. This makes the x86 PC
architecture a Tier 1 supported platform.

Availible Drivers
-----------------

* [VGA Console](../../../drivers/console/vga/)
* [16550 Compatible UART](../../../drivers/console/16550_uart/)
* [Real Time Clock](../../../drivers/clock/rtc-cmos)
* [Intel 8254 PIC](../../../drivers/clock/i8254)
