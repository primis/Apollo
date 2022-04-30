### The Apollo Project
_(C) 2013 - 2022 Primis Computers (Allie Sargente et al)_
![](https://img.shields.io/apm/l/vim-mode.svg)

Requirements:
=================================================
This is a kernel project for x86 Hardware. Current compilation needs:
* GNU Make (Or Compatible)
* NASM, The Netwide Assembler
* GCC (cross compiler heavily suggested, such as i586-elf-gcc)

Optionally, for customizing builds, any application capable of reading Kconfig
files will work. If on debian based systems, the package `kconfig-frontends` is suggested.

The kernel is written in X86-Assembly and ANSI-C. We are currently working
on making it as portable as possible.

This software is licensed under the MIT License.
(see License)

Supported Architectures:
=================================================
Currently, only IBM-PC Compatible 486+ PC's are targeted, however this
will change with time.

Notable Features to Consider:
=================================================
This kernel is in development, and is therefore subject to major changes.
The API is in a state of flux. If developing applications, stick to
functions from the standard c library. All others may be depreciated /
removed without notice.

Running
=================================================
The best way to run latest is to use the ipxe images. 
These should work on any VM/PC with a network card and
will load the latest kernel from kernel.primis.org
You can also load x86-sys.mod from any multiboot
compatible bootloader such as GRUB.

Building
=================================================
Once all the pre-requisites software has been aquired,
you can build a kernel simply by typing `make`. The make
file defaults to x86 with most of the features set.
To customize features you can either directly edit the
`.config` file in the root directory of the project, or
you can run `make menuconfig` if you have `kconfig-frontends`
or it's equivilent package installed on your distro.

History:
=================================================
This kernel descends from several failed kernel projects;
* ScorchOS; by Bob Moss and Allie Sargente. Circa 2010
* Mercury/SaturnOS; by Brandon Cornell and Allie Sargente. Circa 2012
* Apollo Kernel Project; Circa 2009-2011. Allie Sargente.

This kernel takes base code from Bkerndev and James Molloy's Tutorials:
* http://www.osdever.net/bkerndev/Docs/title.htm
* http://www.jamesmolloy.co.uk/tutorial_html/index.html

Greets to the OSDev Community at OSdev.org.

