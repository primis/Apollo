#include <sys/hal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef GITREV
#define GITREV 1
#endif

extern uint32_t free_memory_get_megs();

void kmain(int argc, char **argv)
{
    int i;
    int proc_count = get_num_processors();
    int major = hal_version_major();
    int minor = hal_version_minor();

    printf("Apollo Kernel Version %d.%d (Build %s).\n", major, minor, GITREV);
    printf("%d System Processor", proc_count);

    if(proc_count > 1)
    {
        printf("s");
    }

    printf(" [%d MB Memory]\n", free_memory_get_megs());
    
    printf("\nKernel Arguments:\n");
    
    for(i = 0; i < argc; i++)
    {
        printf(" [%d] = %s\n", i, argv[i]);
    }

    while(1);
}
