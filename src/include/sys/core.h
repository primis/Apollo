/*
 * (c) 2018 Apollo Project Developers
 * core.h - A place for all the stray function names
 */

#ifndef __SYS_CORE_H
#define __SYS_CORE_H

#include <sys/multiboot.h>
#define KSTALLOC_SZ 4098

void multiboot_parse(multiboot_info_t*, multiboot_info_t*);
void main();
unsigned int kstalloc(unsigned int len);
#endif
