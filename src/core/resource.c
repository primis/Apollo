#include <sys/resource.h>
#include <string.h>
#include <errno.h>

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
            return -ENOTSUPP;
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
            return -ENOTSUPP;
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
