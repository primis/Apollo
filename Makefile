#############################
## Apollo Project Makefile ##
#############################
.PHONY: all clean link

# Git revision number
GIT_REV		!= git rev-parse --short HEAD 2>/dev/null |tr '[:lower:]' '[:upper:]'
BUILD		:= build

# Documentation
MDSOURCES	!= find docs -type f -name '*.md'
MARKDOWN 	:= pandoc
HTMLDIR		:= html
MDFLAGS		:= --from gfm --to html --standalone \
		--metadata title="Apollo Kernel Documentation"
HTMLDOCS	:= $(patsubst docs/%.md,$(HTMLDIR)/%.html,$(MDSOURCES))
HTMLDIRS 	:= $(shell cd docs && find -type d | tr '\n' ' ')

INCLUDEDIR	!= find src -type d -name "include" -printf "-I%p "

include .config
TARGETL		!= echo $(CONFIG_TARGET) | tr '[:upper:]' '[:lower:]'
BUILD 		:= build-$(TARGETL)
BIN			:= kernel.mod
include $(shell pwd)/scripts/target-$(TARGETL).mk

WARNINGS	:= -Wall -Wextra -Wno-unused-parameter
DEFS		:= $(INCLUDEDIR) -fbuiltin -DGITREV="\"$(GIT_REV)\""

HELPER_MK	:= $(shell pwd)/scripts/helper.mk
MK_FLAGS	:= --no-print-directory -s
SUBDIRS		:= $(shell find . -name Makefile)
SUBDIRS		:= $(patsubst %Makefile,%,$(SUBDIRS))
SUBDIRS	 	:= $(patsubst %./,%,$(SUBDIRS))
OBJECTS		:= $(shell for dir in $(SUBDIRS); do\
				 TOP_DIR=`pwd` $(MAKE) $(MK_FLAGS) -f $(HELPER_MK) -C $$dir;\
				 done;)

ifeq ($(CONFIG_TESTING),y)
	DEFS	+= -DTEST_HARNESS=1
	BIN		:= test-$(BIN)
endif

OBJECTS		:= $(patsubst %.o,$(BUILD)/%.o,$(OBJECTS))
SRCDIR		!= find src/ -type d | tr '\n' ' '

all: link doc

# C files are compiled universally the same, assembler targets are defined in
# The target specific makefiles
$(BUILD)/%.c.o: %.c Makefile | setup_builddir
	@printf "\033[1mCC\033[0m   $<\n"
	@$(CC) -c $< -o $@ $(DEFS) $(TARGET_DEFS) $(WARNINGS)

link: $(OBJECTS)
	@printf "\033[1mLINK\033[0m $@\n"
	@$(CC) $(DEFS) $(WARNINGS) $(LDFLAGS) $(TARGET_LDFLAGS) \
	-o $(BUILD)/apollo/$(BIN) $(OBJECTS)

setup_builddir:
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/apollo
	@cd $(BUILD) && mkdir -p `echo $(SRCDIR)`

clean:
	@printf "\033[1mCLEAN\033[0m \n"
	@find . -type d -name "build*" -exec rm -rf {} +
	@find $(HTMLDIR) -type f -exec rm {} +

doc_setup:
	@mkdir -p $(HTMLDIR)
	@cd $(HTMLDIR) && mkdir -p $(HTMLDIRS)

doc: doc_setup | $(HTMLDOCS)

html/%.html: docs/%.md
	@printf "\033[1mHTML\033[0m $<\n"
	@$(MARKDOWN) $(MDFLAGS) $< --output $@

menuconfig:
	@kconfig-mconf Kconfig

iso:
	@cd $(BUILD) && ../scripts/isocreate.sh
