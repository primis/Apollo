#############################
## Apollo Project Makefile ##
#############################
.PHONY: all clean link



# Git revision number
GIT_REV		!= git rev-parse --short HEAD 2> /dev/null
BUILD		:= build
TARGETS		!= find . -maxdepth 1 -type f -name '*.mk'
TARGETS		!= echo $(TARGETS) | sed -e 's/.mk//' -e 's/.\/target-//'

# Documentation
MDSOURCES	!= find doc -type f -name '*.md'
MARKDOWN 	:= pandoc
HTMLDIR		:= html
MDFLAGS		:= --from markdown_github --to html --standalone
HTMLDOCS	:= $(patsubst doc/%.md,$(HTMLDIR)/%.html,$(MDSOURCES))
HTMLDIRS 	:= $(shell cd doc && find -type d | tr '\n' ' ')


# Platform Agnostic Source Code
CSOURCES_TI	!= find src/core src/libc src/adt -type f -name '*.c'

#Third Party Source Code
CSOURCES_TP	!= find src/third_party -type f -name '*.c'

CSOURCES_TI	:= $(CSOURCES_TI) $(CSOURCES_TP)

INCLUDEDIR	!= find src -type d -name "include" -printf "-I%p "

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
DEFS		:= $(INCLUDEDIR) -fbuiltin -DGITREV="\"$(GIT_REV)\""\



all: link doc

# C files are compiled universally the same, assembler targets are defined in
# The target specific makefiles
$(BUILD)/%.c.o: %.c Makefile | setup_builddir
	@printf "\033[1mCC\033[0m   $<\n"
	@$(CC) -c $< -o $@ $(DEFS) $(TARGET_DEFS) $(WARNINGS)

link: $(SOBJECTS) $(COBJECTS)
	@printf "\033[1mLINK\033[0m $@\n"
	@$(CC) $(DEFS) $(TDEFS) $(WARNINGS) $(LDFLAGS) $(TARGET_LDFLAGS) \
	-o bin/$(BIN) $(SOBJECTS) $(COBJECTS)

setup_builddir:
	@mkdir -p $(BUILD)
	@cd $(BUILD) && mkdir -p `echo $(SRCDIR)`

clean:
	@printf "\033[1mCLEAN\033[0m \n"
	@find $(BUILD) -type f -name '*.o' -exec rm {} +
	@find $(HTMLDIR) -type f -exec rm {} +


test: DEFS += -DTEST_HARNESS=1
test: link


doc_setup:
	@mkdir -p $(HTMLDIR)
	@cd $(HTMLDIR) && mkdir -p $(HTMLDIRS)

doc: doc_setup | $(HTMLDOCS)

html/%.html: doc/%.md
	@printf "\033[1mHTML\033[0m $<\n"
	@$(MARKDOWN) $(MDFLAGS) $< --output $@
