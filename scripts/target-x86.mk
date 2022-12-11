# Copyright (c) 2022 Apollo Developers
# For terms, see LICENSE
# Makefile for Apollo Project
# Target is x86

# Check for different Cross compilers in reverse order
# We need to make the shell bash for "command -v" to work
SHELL	:= $(shell which bash)
STRIP	:= $(shell which strip)
CC		:= $(shell command -v i586-elf-gcc)
ifndef CC
CC		:= $(shell command -v i686-elf-gcc)
endif

# Fall back case to regular GCC
ifndef CC
CC		:= gcc
TARGET_LDFLAGS	:= -m32 -Tsrc/arch/x86/x86-link.ld -nostdlib
$(warning Using default GCC, please consider installing a cross compiler!)
endif

AS				:= nasm
ASFLAGS			:= -felf
TARGET_CFLAGS 	:= -DX86=1 -m32 -masm=intel -ffreestanding -nostdlib -Os
TARGET_LDFLAGS	?= -m32 -Tsrc/arch/x86/link.ld -nostdlib -lgcc

$(BUILD)/%.s.o: %.s
	@printf "\033[1mAS\033[0m   $<\n"
	@mkdir -p $(@D)
	@$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/%.iso: $(BUILD)/apollo/$(BIN)
	@$(STRIP) -s $<
	@cd $(BUILD) && ../scripts/grub-isocreate.sh apollo/$(BIN)
