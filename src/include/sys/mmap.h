#ifndef __SYS_MMAP_H
#define __SYS_MMAP_H

// This file steps to includes for arch specific mmap's

#if defined(X64)
#include <arch/x64/mmap.h>
#elif defined(X86)
#include <arch/x86/mmap.h>
#elif defined(HOSTED)
#include <arch/hosted/mmap.h>
#endif

#endif
