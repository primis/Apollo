# Makefile for Apollo Project
# Target is x86
# Copyright 2018 Apollo Developers

CC				:= i586-elf-gcc
BIN				:= x86-sys.mod
AS				:= nasm
ASFLAGS			:= -felf
TARGET_DEFS 	:= -DX86=1 -m32 -masm=intel -ffreestanding -nostdlib
TARGET_LDFLAGS	:= -m32 -Tsrc/arch/x86/x86-link.ld -nostdlib -lgcc -n
CSOURCES	:= $(shell find src/arch/x86 -type f -name "*.c") $(CSOURCES_TI)
SSOURCES 	:= $(shell find src/arch/x86 -type f -name "*.s") $(SSOURCES_TI)

$(BUILD)/%.s.o: %.s | setup_builddir
	@printf "\033[1mAS\033[0m   $<\n"
	@$(AS) $(ASFLAGS) $< -o $@
