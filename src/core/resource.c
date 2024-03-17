#include <sys/resource.h>
#include <string.h>
#include <errno.h>

#define weak __attribute__((__weak__))

int resource_io_write(void *src, resource_t *r,
resource_type_t off, size_t n) weak;
int resource_io_write(void *src, resource_t *r, resource_type_t off, size_t n)
{
    return -ENOTSUPP;
}

int resource_io_read(void *dest, resource_t *r,
resource_type_t off, size_t n) weak;
int resource_io_read(void *dest, resource_t *r, resource_type_t off, size_t n)
{
    return -ENOTSUPP;
}

static void resource_mem_read_8(void *buf, void *reg)
{
    uint8_t *dest = (uint8_t *)buf;
    volatile uint8_t *src = (volatile uint8_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_read_16(void *buf, void *reg)
{
    uint16_t *dest = (uint16_t *)buf;
    volatile uint16_t *src = (volatile uint16_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_read_32(void *buf, void *reg)
{
    uint32_t *dest = (uint32_t *)buf;
    volatile uint32_t *src = (volatile uint32_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_read_64(void *buf, void *reg)
{
    uint64_t *dest = (uint64_t *)buf;
    volatile uint64_t *src = (volatile uint64_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_write_8(void *reg, void *buf)
{
    uint8_t *src = (uint8_t *)buf;
    volatile uint8_t *dest = (volatile uint8_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_write_16(void *reg, void *buf)
{
    uint16_t *src = (uint16_t *)buf;
    volatile uint16_t *dest = (volatile uint16_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_write_32(void *reg, void *buf)
{
    uint32_t *src = (uint32_t *)buf;
    volatile uint32_t *dest = (volatile uint32_t *)reg;
    *(dest) = *(src);
}

static void resource_mem_write_64(void *reg, void *buf)
{
    uint64_t *src = (uint64_t *)buf;
    volatile uint64_t *dest = (volatile uint64_t *)reg;
    *(dest) = *(src);
}

static int resource_mem_write(void *buf, resource_t *r, resource_type_t off,
size_t n)
{
    size_t i, width = 1;
    void(*io_op)(void *, void *);

    switch (r->flags & RESOURCE_WIDTH)
    {
        case RESOURCE_WIDTH_8:
            width = 1;
            io_op = resource_mem_write_8;
            break;
        case RESOURCE_WIDTH_16:
            width = 2;
            io_op = resource_mem_write_16;
            break;
        case RESOURCE_WIDTH_32:
            width = 4;
            io_op = resource_mem_write_32;
            break;
        case RESOURCE_WIDTH_64:
            width = 8;
            io_op = resource_mem_write_64;
            break;
        default:
            return -EIO;
    }

    off *= width;
    off += r->start;

    for(i = 0; i < n; i += width)
    {
        io_op((void *)(off + i), (buf +i));
    }
    return 0;
}

static int resource_mem_read(void *buf, resource_t *r, resource_type_t off,
size_t n)
{
    size_t i, width = 1;
    void(*io_op)(void *, void *);

    switch (r->flags & RESOURCE_WIDTH)
    {
        case RESOURCE_WIDTH_8:
            width = 1;
            io_op = resource_mem_read_8;
            break;
        case RESOURCE_WIDTH_16:
            width = 2;
            io_op = resource_mem_read_16;
            break;
        case RESOURCE_WIDTH_32:
            width = 4;
            io_op = resource_mem_read_32;
            break;
        case RESOURCE_WIDTH_64:
            width = 8;
            io_op = resource_mem_read_64;
            break;
        default:
            return -EIO;
    }

    off *= width;
    off += r->start;

    for(i = 0; i < n; i += width)
    {
        io_op((buf +i), (void *)(off + i));
    }
    return 0;
}

/**
 * @brief Perform a data write to a resource.
 *
 * @param dest Buffer which contains data to write
 * @param r Resource to write to
 * @param off Offset from start of resource to start writing to
 * @param n Number of elements to write (size is determined by resource flags)
 * @returns 0 on success, else negative errno
 */
int resource_write(void *src, resource_t *r, resource_type_t off, size_t n)
{
    if(src == NULL || r == NULL)
    {
        return -EINVAL;
    }

    switch (r->flags & RESOURCE_TYPE)
    {
        case RESOURCE_MEM:
            return resource_mem_write(src, r, off, n);
        case RESOURCE_IO:
            return resource_io_write(src, r, off, n);
    }

    return -ENOTSUPP;
}

/**
 * @brief Perform a data read of a resource.
 *
 * @param dest Buffer to store read results
 * @param r Resource to read from
 * @param off Offset from start of resource to start reading from
 * @param n Number of elements to read (size is determined by resource flags)
 * @returns 0 on success, else negative errno
 */
int resource_read(void *dest, resource_t *r, resource_type_t off, size_t n)
{
    if(dest == NULL || r == NULL)
    {
        return -EINVAL;
    }

    switch (r->flags & RESOURCE_TYPE)
    {
        case RESOURCE_MEM:
            return resource_mem_read(dest, r, off, n);
        case RESOURCE_IO:
            return resource_io_read(dest, r, off, n);
    }

    return -ENOTSUPP;
}

/**
 * @brief Register a resource with a parent node
 *
 * @param r The resource to register
 * @param parent The parent resource to add this to.
 */
void resource_register(resource_t *r, resource_t *parent)
{
    resource_t *next;
    if(parent == NULL)
    {
        return; // Not linking this resource to a parent, end early
    }

    r->parent = parent;

    if(parent->child == NULL)
    {
        parent->child = r;
        return;             // Parent has no children yet, add one
    }
    next = parent->child;
    while(next->sibling != NULL)
    {
        next = next->sibling;   // Iterate through children until the youngest
    }
    next->sibling = r;          // Become new youngest sibling
}

/**
 * @brief Find a resource by it's name
 *
 * @todo Add recursive search
 *
 * @param name The name of the resource to find
 * @param parent The parent node to search against
 * @return resource_t* NULL on failure, else the resource requested
 */
resource_t *resource_find(const char *name, resource_t *parent)
{
    parent = parent->child;
    while(strcmp(name, parent->name)) {
        parent = parent->sibling;
        if(parent == NULL) {
            return NULL;
        }
    }
    return parent;
}
