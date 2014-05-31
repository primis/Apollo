# Kerenl's Makefile
LD 			= ld -melf_i386
LDFLAGS 	= -Tlink.ld


####### x86 Arch Stuff ##############
x86_PROG	= x86_sys.mod
x86_TRGTS	= core.o libc.o arch/x86.o
x86: $(x86_TRGTS) x86_link
$(x86_PROG): $(x86) link


####### Core Makefile Targets #######
core.o:
	cd ./core ; make core.o
libc.o:
	cd ./libc ; make libc.o
### Archetecture Specific Targets ###
arch/x86.o:
	cd ./arch/x86 ; make x86.o

clean:
	rm -f *.o *~
	cd ./core ; make clean
	cd ./libc ; make clean
	cd ./arch/x86 ; make cleanit

x86_link:
	$(LD) $(LDFLAGS) -o ../bin/sys.mod $(x86_TRGTS)
