################################################################################
## Apollo Kernel Makefile                                                     ## 
################################################################################

X86_C_SOURCES = $(shell find arch/x86 -type f -name '*.c')
X86_S_SOURCES = $(shell find arch/x86 -type f -name '*.s')
X86_OBJECTS   = $(X86_S_SOURCES:.s=.o) $(X86_C_SOURCES:.c=.o)

LIBC_SOURCES  = $(shell find libc -type f -name '*.c')
LIBC_OBJECTS  = $(LIBC_SOURCES:.c=.o)

CORE_SOURCES  = $(shell find core -type f -name '*.c')
CORE_OBJECTS  = $(CORE_SOURCES:.c=.o)

# Kerenl's Makefile
CC			= i586-elf-gcc
AS			= nasm

#### Flags ####
LDFLAGS		= -Tlink.ld -lgcc
CFLAGS		= -Wall -ffreestanding -fbuiltin -nostdlib -masm=intel -I./include
ASFLAGS		= -felf
BINDIR		= ../bin/
### Targets ###
all: $(X86_OBJECTS) $(CORE_OBJECTS) $(LIBC_OBJECTS) link
clean:
	find . -type f -name '*.o' -exec rm {} +
link:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BINDIR)sys.mod $(X86_OBJECTS) $(CORE_OBJECTS) $(LIBC_OBJECTS)
.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)
.s.o:
	$(AS) $(ASFLAGS) $< -o $@
