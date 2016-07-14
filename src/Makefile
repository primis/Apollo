################################################################################
## Apollo Kernel Makefile                                                     ##
################################################################################
.PHONY: all clean link

# Git Revision for build
GIT_REV	      != git rev-parse --short HEAD 2> /dev/null
# Source files

X86_C_SOURCES != find arch/x86 -type f -name '*.c'
X86_S_SOURCES != find arch/x86 -type f -name '*.s'
X86_OBJECTS   = $(X86_S_SOURCES:.s=.o) $(X86_C_SOURCES:.c=.o)

LIBC_SOURCES  != find libc -type f -name '*.c'
LIBC_OBJECTS  = $(LIBC_SOURCES:.c=.o)

CORE_SOURCES  != find core -type f -name '*.c'
CORE_OBJECTS  = $(CORE_SOURCES:.c=.o)

# Default Programs
CC		= i586-elf-gcc
NASM		= nasm

#### Flags ####
LDFLAGS		?= -Tlink.ld -lgcc
CFLAGS		?= -Wall -ffreestanding -fbuiltin -nostdlib -masm=intel
CPPFLAGS	?= -Iinclude -DGITREV="\"$(GIT_REV)\""
NASMFLAGS	?= -felf
BINDIR		?= ../bin/

### Targets ###
all: link
clean:
	find . -type f -name '*.o' -exec rm {} +
link: $(X86_OBJECTS) $(CORE_OBJECTS) $(LIBC_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINDIR)sys.mod $(X86_OBJECTS) $(CORE_OBJECTS) $(LIBC_OBJECTS)
.c.o:
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)
.s.o:
	$(NASM) $(NASMFLAGS) $< -o $@
