# Hosted Makefile for doing testing on hosted systems

BIN			:= hosted-sys.mod
TARGET_DEFS	:= -DHOSTED=1
CSOURCES	:= $(shell find src/arch/hosted -type f -name "*.c") $(CSOURCES_TI)

# That's it!
