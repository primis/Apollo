/*
 * (c) 2018 Apollo Project Developers
 * <types.h> - Hardware Abstraction Layer
 *
 * Includes all of the builtin types
 * Also defines null
 */

#ifndef __TYPES_H
#define __TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#undef NULL
#define NULL 0

typedef size_t phys_addr_t;

typedef phys_addr_t resource_type_t;

#endif
