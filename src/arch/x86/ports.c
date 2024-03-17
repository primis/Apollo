/*
 * (c) 2024 Apollo Project Developers
 * For terms, see LICENSE
 * ports.c - x86 I/O databus commends
 */

#include <stdint.h>
#include <sys/resource.h>
#include <errno.h>
#include <arch/x86/regs.h>

void iowait(void)
{
    // Port 0x80 is used for POST checkpoints, so it's safe to use
    // As a "scrap" port to throw data to as a bus wait.
    // On a side note, port 0x80 shows up on certain MoBo's as a diagnostic
    // for POST Codes, so that's kinda cool.
    __asm__ __volatile__ ("outb 0x80, %0" : : "a" ((unsigned char)0));
}

static void io_write_8(uint16_t port, void* data)
{
    uint8_t value = *(uint8_t *)data;
    __asm__ __volatile__ ("outb %0, %1" : : "dN" (port), "a" (value));
}

static void io_write_16(uint16_t port, void* data)
{
    uint16_t value = *(uint16_t *)data;
    __asm__ __volatile__ ("outw %0, %1" : : "dN" (port), "a" (value));
}

static void io_write_32(uint16_t port, void* data)
{
    uint32_t value = *(uint32_t *)data;
    __asm__ __volatile__ ("out %0, %1" : : "dN" (port), "a" (value));
}

static void io_read_8(uint16_t port, void* data)
{
    uint8_t ret;
    __asm__ __volatile__ ("inb %0, %1" : "=a" (ret) : "dN" (port));
    *(uint8_t*)data = ret;
}

static void io_read_16(uint16_t port, void* data)
{
    uint16_t ret;
    __asm__ __volatile__ ("inw %0, %1" : "=a" (ret) : "dN" (port));
    *(uint16_t*)data = ret;
}

static void io_read_32(uint16_t port, void* data)
{
    uint32_t ret;
    __asm__ __volatile__ ("in %0, %1" : "=a" (ret) : "dN" (port));
    *(uint32_t*)data = ret;
}

static int resource_io_data_op(void *src, resource_t *r, resource_type_t off,
    size_t n, int read)
{
    int width;
    size_t i;
    void (*io_op)(uint16_t, void *);
    if(read == 1)
    {
        switch(r->flags & RESOURCE_WIDTH)
        {
            case RESOURCE_WIDTH_8:
                io_op = io_read_8;
                width = 1;
                break;
            case RESOURCE_WIDTH_16:
                io_op = io_read_16;
                width = 2;
                break;
            case RESOURCE_WIDTH_32:
                io_op = io_read_32;
                width = 4;
                break;
            default:
                return -EIO;
        }
    }

    else
    {
        switch (r->flags & RESOURCE_WIDTH)
        {
            case RESOURCE_WIDTH_8:
                io_op = io_write_8;
                width = 1;
                break;
            case RESOURCE_WIDTH_16:
                io_op = io_write_16;
                width = 2;
                break;
            case RESOURCE_WIDTH_32:
                io_op = io_write_32;
                width = 4;
                break;
            default:
                return -EIO;
        }
    }
    switch(r->flags & 0x0F)
    {
        case RESOURCE_IO_LINEAR:    /* Standard linear addressing IO Write */
            off += r->start;
            for(i = 0; i < n; i += width)
            {
                if(r->flags & RESOURCE_IO_SLOW)
                {
                    iowait();
                }
                io_op(off + i, (src + i));
            }
            break;

        case RESOURCE_IO_FIFO:  /* Write to the same port, over and over */
            for(i = 0; i < n; i++)
            {
                if(r->flags & RESOURCE_IO_SLOW)
                {
                    iowait();
                }

                io_op(r->start, (src + i));
            }
            break;

        case RESOURCE_IO_INDEXED: /* Write to the indexing port, then data */
            for(i = 0; i < n; i++)
            {
                uint8_t index = (off + i);
                if(r->flags & RESOURCE_IO_SLOW)
                {
                    iowait();
                }
                io_write_8(r->start, &index);
                io_op(r->end, (src + i));
            }
            break;

        default:
            return -ENOTSUPP;
    }

    return 0;
}

int resource_io_write(void *src, resource_t *r, resource_type_t off, size_t n)
{
    return resource_io_data_op(src, r, off, n, 0);
}

int resource_io_read(void *dest, resource_t *r, resource_type_t off, size_t n)
{
        return resource_io_data_op(dest, r, off, n, 1);
}

inline uint32_t read_cr0()
{
    uint32_t ret;
    __asm__ __volatile__ ("mov %0, cr0" : "=r" (ret));
    return ret;
}

inline uint32_t read_cr2()
{
    uint32_t ret;
    __asm__ __volatile__ ("mov %0, cr2" : "=r" (ret));
    return ret;
}

inline uint32_t read_cr3()
{
    uint32_t ret;
    __asm__ __volatile__ ("mov %0, cr3" : "=r" (ret));
    return ret;
}

inline void write_cr0(uint32_t val)
{
    __asm__ __volatile__ ("mov cr0, %0" : : "r" (val));
}

inline void write_cr2(uint32_t val)
{
    __asm__ __volatile__ ("mov cr2, %0" : : "r" (val));
}

inline void write_cr3(uint32_t val)
{
    __asm__ __volatile__ ("mov cr3, %0" : : "r" (val));
}
