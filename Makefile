#############################
## Apollo Project Makefile ##
#############################
.PHONY: all clean link

# Git revision number
GIT_REV		!= git rev-parse --short HEAD 2> /dev/null
BUILD		:= build
TARGETS		!= find . -maxdepth 1 -type f -name '*.mk' 
TARGETS		!= echo $(TARGETS) | sed -e 's/.mk//' -e 's/.\/target-//'

# Platform Agnostic Source Code
CSOURCES_TI	!= find src/core src/libc src/adt -type f -name '*.c'

all:

ifndef TARGET
	# Not a fatal error if clean or doc
	ifeq($(MAKECMDGOALS),clean)
	else ifeq ($(MAKECMDGOALS),doc)
	else
		$(error TARGET is not set, availible targets: $(TARGETS))
endif

TARGETL		!= echo $(TARGET) | tr '[:upper:]' '[:lower:]'

ifdef TARGET
	BUILD 	:= build-$(TARGETL)
-include target-$(TARGETL).mk
endif

COBJECTS	:= $(patsubst %.c,$(BUILD)/%.c.o,$(CSOURCES))
SOBJECTS	:= $(patsubst %.s,$(BUILD)/%.s.o,$(SSOURCES))
SRCDIR		!= find src/ -type d | tr '\n' ' '

WARNINGS	:= -Wall -Wextra -Wno-unused-parameter
DEFS		:= -fbuiltin -DGITREV="\"$(GIT_REV)\""\
		-Isrc/include -O0 -std=c99

all: link

# C files are compiled universally the same, assembler targets are defined in
# The target specific makefiles
$(BUILD)/%.c.o: %.c Makefile | setup_builddir 
	@printf "\033[1mCC\033[0m   $<\n"
	@$(CC) -c $< -o $@ $(DEFS) $(TARGET_DEFS) $(WARNINGS) 

link: $(SOBJECTS) $(COBJECTS)
	@printf "\033[1mLINK\033[0m $@\n"
	@$(CC) $(DEFS) $(WARNINGS) $(LDFLAGS) $(TARGET_LDFLAGS) -o bin/$(BIN) \
	$(SOBJECTS) $(COBJECTS)

setup_builddir:
	@mkdir -p $(BUILD)
	@cd $(BUILD) && mkdir -p `echo $(SRCDIR)`

clean: 
	@printf "\033[1mCLEAN\033[0m \n"
	@find $(BUILD) -type f -name '*.o' -exec rm {} +
