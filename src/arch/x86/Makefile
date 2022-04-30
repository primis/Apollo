##############################
# Apollo Project             #
# x86 Arch specific Makefile #
# (c) 2022 Apollo Developers #
##############################

obj_$(CONFIG_X86_BASE) := start.s multiboot.s 
obj_$(CONFIG_X86_BASE) += bringup.c ports.c
obj_$(CONFIG_X86_BASE) += idt.c irq.c ivt.s
obj_$(CONFIG_X86_BASE) += gdt.c
obj_$(CONFIG_X86_BASE) += free_memory.c vmm.c