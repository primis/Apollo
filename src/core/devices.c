/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * Device bootstrap and initialization
 */

#include <string.h>
#include <errno.h>
#include <sys/device.h>

extern device_driver_t __drivers_begin, __drivers_end;

static device_t root_device = {
    .name = "sys",
    .parent = NULL
};

static device_driver_t *find_driver(const char *name)
{
    for(device_driver_t *i = &__drivers_begin, *e = &__drivers_end;
        i < e; i++) {
        if (!strcmp(name, i->compat)) {
            return i;
        }
    }
    return NULL;
}

static void device_register_child(device_t *p, device_t *c)
{
    if(p == NULL) {
        return;
    }
    c->parent = p;

    if(p->child == NULL) {
        p->child = c;
        return;
    }

    p = p->child;
    while(p->sibling != NULL) {
        p = p->sibling;
    }
    p->sibling = c;
}

static void device_unregister_child(device_t *p, device_t *c)
{
    if(p == NULL) {
        return;
    }

    if(p->child == c) {
        p->child = NULL;
        return;
    }

    p = p->child;
    while (p->sibling != c) {
        if(p->sibling == NULL) {
            return;
        }
        p = p->sibling;
    }

    p->sibling = c->sibling;
}

device_t *base_device()
{
    return &root_device;
}

int device_register(device_t *device, device_t *parent, const char *compat)
{
    if(device == NULL) {
        return -EINVAL;
    }
    device->driver = find_driver(compat);

    device_register_child(parent, device);

    return 0;
}

int device_init(device_t *device, void *config)
{
    if(device == NULL) {
        return -EINVAL;
    }

    if(device->driver == NULL) {
        return -ENODEV;
    }

    if(device->driver->init == NULL) {
        return -EOPNOTSUPP;
    }

    return device->driver->init(device, config);
}

int device_unregister(device_t *device)
{
    if(device == NULL) {
        return -ENODEV;
    }

    while(device->child != NULL) {
        device_unregister(device->child);
        device->child = device->child->sibling;
    }

    device_unregister_child(device->parent, device);

    if(device->driver != NULL) {
        if(device->driver->fini != NULL) {
            return device->driver->fini(device);
        }
    }
    return 0;
}

int device_api_call(device_t *device, void *data)
{
    if(device == NULL) {
        return -EINVAL;
    }
    if(device->driver == NULL) {
        return -ENODEV;
    }
    if(device->driver->api == NULL) {
        return -ENOSYS;
    }
    return device->driver->api(device, data);
}

/* Driver for the root device, this is here so the linker doesn't yell at us */
DRIVER = {
    .compat = "root",
    .init   = NULL,
    .fini   = NULL,
    .api    = NULL
};
