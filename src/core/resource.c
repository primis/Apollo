#include <sys/resource.h>

#define weak __attribute__((__weak__))

int resource_io_write(void *src, resource_t *r, 
resource_type_t off, size_t n) weak;
int resource_io_write(void *src, resource_t *r, resource_type_t off, size_t n) 
{
    return -1; 
}

int resource_io_read(void *dest, resource_t *r, 
resource_type_t off, size_t n) weak;
int resource_io_read(void *dest, resource_t *r, resource_type_t off, size_t n)
{
    return -1;
}

int resource_write(void *src, resource_t *r, resource_type_t off, size_t n)
{
    if(src == NULL || r == NULL)
    {
        return -1;
    }

    switch (r->flags & RESOURCE_TYPE)
    {
        case RESOURCE_MEM:
            return -1;
        case RESOURCE_IO:
            return resource_io_write(src, r, off, n);
    }
    
    return -1;
}

int resource_read(void *dest, resource_t *r, resource_type_t off, size_t n)
{
    if(dest == NULL || r == NULL)
    {
        return -1;
    }
    
    switch (r->flags & RESOURCE_TYPE)
    {
        case RESOURCE_MEM:
            return -1;
        case RESOURCE_IO:
            return resource_io_read(dest, r, off, n);
    }

    return -1;
}
