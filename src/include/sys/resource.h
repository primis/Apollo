#ifndef __SYS_RESOURCE_H
#define __SYS_RESOURCE_H

#include <types.h>

/* Bits 8-11: Resource Type */
#define RESOURCE_TYPE       0x00000F00
#define RESOURCE_IO         0x00000100  /* IO Bus */
#define RESOURCE_MEM        0x00000200  /* Memory Bus */

/* Bits 0-7 are Resource type specific */
#define RESOURCE_SPECIFIC   0x000000FF

/* Bits 0-7 Resource IO Specific */
/* Bits 0-3 are IO Resource Types */
#define RESOURCE_IO_LINEAR  0x00000000  /* Linear Access of resource */
#define RESOURCE_IO_INDEXED 0x00000001  /* Access is controlled via an index */
#define RESOURCE_IO_FIFO    0x00000002  /* Resource is a FIFO buffer */
/* Bits 4-7 are Type Modifiers */
#define RESOURCE_IO_SLOW    0x00000010  /* Access requires an iowait() */

/* Bits 12 & 13: Resource Data Width */
#define RESOURCE_WIDTH      0x00003000
#define RESOURCE_WIDTH_8    0x00000000  /* (Default) 8 bit data */
#define RESOURCE_WIDTH_16   0x00001000  /* 16 Bit Data transfer */
#define RESOURCE_WIDTH_32   0x00002000  /* 32 Bit Data transfer */
#define RESOURCE_WIDTH_64   0x00003000  /* 64 Bit Data transfer */

/* Generic */
#define RESOURCE_DISABLED   0x10000000  /* Resource unreachable */ 
#define RESOURCE_BUSY       0x20000000  /* Resource in use */

typedef struct resource {
    resource_type_t start;
    resource_type_t end;
    const char *name;
    uint32_t flags;
    struct resource *parent, *sibling, *child; 
} resource_t;

int resource_write(void *src, resource_t *r, resource_type_t off, size_t n);
int resource_read(void *dest, resource_t *r, resource_type_t off, size_t n);

#endif
