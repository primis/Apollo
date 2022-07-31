# Makefile for Apollo Project
# Target is x86
# Copyright 2022 Apollo Developers


# Check for different Cross compilers in reverse order
# We need to make the shell bash for "command -v" to work
SHELL	:= $(shell which bash)
CC		:= $(shell command -v i586-elf-gcc)
ifndef CC
CC		:= $(shell command -v i686-elf-gcc)
endif

# Fall back case to regular GCC
ifndef CC
CC		:= gcc
TARGET_LDFLAGS	:= -m32 -Tsrc/arch/x86/x86-link.ld -nostdlib -n
$(warning Using default GCC, please consider installing a cross compiler!)
endif

AS				:= nasm
ASFLAGS			:= -felf -dGITREV="'$(GIT_REV)'"
TARGET_DEFS 	:= -DX86=1 -m32 -masm=intel -ffreestanding -nostdlib
TARGET_LDFLAGS	?= -m32 -Tsrc/arch/x86/x86-link.ld -nostdlib -lgcc -n

$(BUILD)/%.s.o: %.s | setup_builddir
	@printf "\033[1mAS\033[0m   $<\n"
	@$(AS) $(ASFLAGS) $< -o $@
