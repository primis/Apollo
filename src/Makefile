# Kerenl's Makefile

PROG		= sys.mod

x86_TRGTS	= core.o arch/x86.o

LD 			= /usr/cross/bin/i586-elf-ld
LDFLAGS 	= -Tlink.ld

x86: $(x86_TRGTS) x86_link

$(PROG): $(x86) link

core.o:
	cd ./core ; make core.o
arch/x86.o:
	cd ./arch/x86 ; make x86.o

clean:
	rm -f *.o *~
	cd ./core ; make clean
	cd ./arch/x86 ; make cleanit

x86_link:
	$(LD) $(LDFLAGS) -o ../bin/sys.mod $(x86_TRGTS)
