The Apollo Project
=================
_(C) 2013 - 2024 Primis Computers (Allie Sargente et al)_

Requirements:
--------------------------------------------------

X86 requirements are
* [GNU Make](https://www.gnu.org/software/make/) (Or Compatible)
* [NASM](https://www.nasm.us/), The Netwide Assembler
* [GCC cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler) targeting
    i586-elf or i686-elf
* [grub-mkrescue](https://manpages.ubuntu.com/manpages/bionic/man1/grub-mkrescue.1.html)

RPI4 requirements are
* [GNU Make](https://www.gnu.org/software/make/) (Or Compatible)
* [GCC cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler) targeting
    i586-elf or i686-elf
* [MTools](https://www.gnu.org/software/mtools/)

Optionally, for customizing builds, any application capable of reading Kconfig
files will work. If on debian based systems, the package `kconfig-frontends`
is suggested.

The kernel is written mostly in ANSI-C, with assembly only being used on arch
specific bringup.

This software is licensed under the MIT License.
(see License)

Supported Architectures:
--------------------------------------------------
- IBM-PC Compatible Pentium+ PC's (x86)
- Raspberry Pi Model 4B (rpi4)

Notable Features to Consider:
--------------------------------------------------
This kernel is in development, and is therefore subject to major changes.
The API is in a state of flux. If developing applications, stick to
functions from the standard c library. All others may be deprecated /
removed without notice.

Running
--------------------------------------------------
All targets generate a bootable apollo.iso file.
This file can be used with a virtual machine, burnt to a flash drive or other
bootable media. Please note that for some targets, the iso file is actually a
generic image file, and should not be burned to a CD (for example, for the rpi4)

Configuration
--------------------------------------------------
Once all the pre-requisites software has been aquired for a specific
archetecture, copy the architecture .config file to `.config` at the root of the
project. The provided configuration files are examples only, and can be
customized by running `make menuconfig` if you have a `kconfig` editor installed
on your machine. Debian provides a package called `kconfig-frontends` that fits
this role. You can also edit the .config file by hand, assuming you know what
kernel compilation options you desire.

Building
--------------------------------------------------
You can build the kernel simply by `make`.
After compilation is done, a file called `apollo.iso` will be generated within
the build directory.

History:
--------------------------------------------------
This kernel descends from several failed kernel projects;
* ScorchOS; by Bob Moss and Allie Sargente. Circa 2010
* Mercury/SaturnOS; by Brandon Cornell and Allie Sargente. Circa 2012
* Apollo Kernel Project; Circa 2009-2011. Allie Sargente.

This kernel takes base code from Bkerndev and James Molloy's Tutorials:
* http://www.osdever.net/bkerndev/Docs/title.htm
* http://www.jamesmolloy.co.uk/tutorial_html/index.html

Greets to the OSDev Community at OSdev.org.

