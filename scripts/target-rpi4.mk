# Makefile for Apollo Project
# Target is the Raspberry Pi 4
# Copyright 2022 Apollo Developers


# Check for different Cross compilers in reverse order
# We need to make the shell bash for "command -v" to work
SHELL	:= $(shell which bash)
CC		:= $(shell command -v aarch64-elf-gcc)
OBJCOPY := $(shell command -v aarch64-elf-objcopy)

TARGET_CFLAGS 	:= -DRPI4=1 -ffreestanding -nostdlib -O2
TARGET_LDFLAGS	?= -Tsrc/arch/rpi4/link.ld -nostdlib -lgcc -z max-page-size=4096

$(BUILD)/%.S.o: %.S
	@printf "\033[1mAS\033[0m   $<\n"
	@mkdir -p $(@D)
	@$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.iso: $(BUILD)/apollo/$(BIN)
	@$(OBJCOPY) $(BUILD)/apollo/$(BIN) -O binary $(BUILD)/kernel8.img
	@cd $(BUILD) && ../scripts/rpi4-isocreate.sh kernel8.img 
