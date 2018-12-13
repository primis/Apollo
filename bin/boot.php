#!ipxe

kernel x86-sys.mod

menu iPXE boot menu for Apollo
item --gap --   ----------------------- Operating System -----------------------
item x86        x86 Kernel
item test       x86 Kernel Unit Tests
choose os && goto ${os}

:x86
kernel x86-sys.mod
boot

:test
kernel x86-sys-test.mod
boot
