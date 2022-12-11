/*
 * Copyright (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * <sys/device.h> - Device abstraction layer
 *
 */

#ifndef __SYS_DEVICE_H
#define __SYS_DEVICE_H

#include <sys/resource.h>

typedef struct device_struct device_t;

typedef struct {
    const char *compat;
    int (*init)(device_t *device, void *config);
    int (*fini)(device_t *device);
    int (*api)(device_t *device, void *data);
} device_driver_t;

/**
 * @brief Device instance definition.
 *
 * @name:       Name of this device instance
 * @data:       Private data
 * @access:     Resource pointer to access device
 * @driver:     (semi-opaque) Driver associated with this device.
 * @parent:     (semi-opaque) Parent device in the device tree
 * @sibling:    (semi-opaque) Next sibling device in the device tree
 * @child:      (semi-opaque) Child device in the device tree.
 */
struct device_struct {
    char *name;
    void *data;
    resource_t *access;
    device_driver_t *driver;
    device_t *parent, *sibling, *child;
};

/**
 * @brief Retrieve a pointer to the base device
 *
 * @return device_t*
 */
device_t *base_device();

int device_register(device_t *d, device_t *parent, const char *compat);
int device_unregister(device_t *d);
int device_init(device_t *d, void *config);
int device_api_call(device_t *d, void *data);

#define DRIVER static device_driver_t x \
    __attribute__((__section__(".drivers"),used))

#endif
