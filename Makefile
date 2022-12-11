#############################
## Apollo Project Makefile ##
#############################
.PHONY: all clean link iso

# Git revision number
GIT_REV		!= git rev-parse --short HEAD 2>/dev/null |tr '[:lower:]' '[:upper:]'

INCLUDEDIR	!= find src -type d -name "include" -printf "-I%p "

include .config
TARGETL		!= echo $(CONFIG_TARGET) | tr '[:upper:]' '[:lower:]'
BUILD 		:= build-$(TARGETL)
BIN			:= kernel.mod
include $(shell pwd)/scripts/target-$(TARGETL).mk

WARNINGS	:= -Wall -Wextra -Wno-unused-parameter
CFLAGS		:= $(INCLUDEDIR) -fbuiltin -DGITREV="\"$(GIT_REV)\""

HELPER_MK	:= $(shell pwd)/scripts/helper.mk
MK_FLAGS	:= --no-print-directory -s
SUBDIRS		:= $(shell find . -name Makefile)
SUBDIRS		:= $(patsubst %Makefile,%,$(SUBDIRS))
SUBDIRS	 	:= $(patsubst %./,%,$(SUBDIRS))
OBJECTS		:= $(shell for dir in $(SUBDIRS); do\
				 TOP_DIR=`pwd` $(MAKE) $(MK_FLAGS) -f $(HELPER_MK) -C $$dir;\
				 done;)

ifeq ($(CONFIG_TESTING),y)
	CFLAGS	+= -DTEST_HARNESS=1
endif

OBJECTS		:= $(patsubst %.o,$(BUILD)/%.o,$(OBJECTS))
SRCDIR		!= find src/ -type d | tr '\n' ' '

all: $(BUILD)/apollo.iso

# C files are compiled universally the same, assembler targets are defined in
# The target specific makefiles
$(BUILD)/%.c.o: %.c
	@printf "\033[1mCC\033[0m   $<\n"
	@mkdir -p $(@D)
	@$(CC) -c $< -o $@ $(CFLAGS) $(TARGET_CFLAGS) $(WARNINGS)

$(BUILD)/apollo/$(BIN): $(OBJECTS)
	@printf "\033[1mLINK\033[0m $@\n"
	@mkdir -p $(BUILD)/apollo
	@$(CC) $(LDFLAGS) $(TARGET_LDFLAGS) -o $(BUILD)/apollo/$(BIN) $(OBJECTS)

link: $(BUILD)/apollo/$(BIN)

clean:
	@printf "\033[1mCLEAN\033[0m \n"
	@find . -type d -name "build*" -exec rm -rf {} +
	@rm -rf $(HTMLDIR)

menuconfig:
	@kconfig-mconf Kconfig

iso: $(BUILD)/apollo.iso
