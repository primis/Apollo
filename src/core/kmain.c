
#include <sys/hal.h>
#include <sys/multiboot.h>
#include <stdio.h>
#include <string.h>
extern multiboot_info_t mboot;
void kmain(int argc, char **argv)
{
    int i;
   
    printf("Hello, World!\n");
    for(i = 0; i < argc; i++) 
    {
        printf("Argv[%d] = %s\n", i, argv[i]);
    }
}