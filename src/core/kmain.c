#include <sys/hal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef GITREV
#define GITREV 1
#endif

extern uint32_t free_memory_get_megs();

char* spinny[] = {"|","/","-","\\"};

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
    
    uint64_t t0 = get_timestamp();
    uint64_t t1 = 500000;
    i = 0;
    printf(" ");
    while(1)
    {
        if(get_timestamp() > (t0 + t1))
        {
            i++;
            t0 += t1;
            if(i > 3)
            {
                i = 0;
            }
            printf("\b%s", spinny[i]);
        }
    }
}
