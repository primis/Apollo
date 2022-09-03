#include <stdint.h>

extern int main(int argc, char **argv);

#include <sys/hal.h>
extern module_t __start_modules, __stop_modules;

void arch_init(void)
{
    char *argc[] = {"debug=y"};
    const int argv = 1;
    
    main(argv, argc);
    while (1) {

    }
}

int get_num_processors()
{
    return 4; 
}

uint32_t free_memory_get_megs()
{
    return 1024; // All Raspi4's have at least 1GB ram.
}
