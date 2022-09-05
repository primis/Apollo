/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * bcm2711_gpio.c - Driver to control the GPIO pins of the BCM2711 SoC
 */

#include <sys/device.h>
#include <sys/resource.h>
#include <stdint.h>
#include <errno.h>
#include "include/bcm2711_gpio.h"

static int gpio_call(device_t *dev, uint32_t pin, uint32_t val, int reg, 
    int field_sz, int read)
{
    uint32_t current_value;
    int num_fields          = 32 / field_sz;
    int bitshift            = (pin % num_fields) * field_sz;
    resource_type_t off     = (pin / num_fields) + reg;
    unsigned int field_mask = (1 << field_sz) - 1;
    
    if (pin > GPIO_MAX_PIN) {
        return -EINVAL;
    }
    if (val > field_mask) {
        return -EINVAL;
    }

    if(resource_read(&current_value, dev->access, off, 1)) {
       return -EIO; 
    }

    if(read)
    {
        return (current_value >> bitshift) & field_mask;
    }

    // Clear out bits related to what we're changing
    current_value &= ~(field_mask << bitshift);
    // Inject value into correct position
    current_value |= val << bitshift;
    return resource_write(&current_value, dev->access, off, 1);
}

static int gpio_function(device_t *dev, int pin_number, int value)
{
    return gpio_call(dev, pin_number, value, GPFSEL_REG, GPFSEL_FIELD_SIZE, 0);
}

static int gpio_pull(device_t *dev, int pin_number, int value)
{
    return gpio_call(dev, pin_number, value, GPPULL_REG, GPPULL_FIELD_SIZE, 0);
}

static int api_call(device_t *dev, void *api_data)
{
    int32_t *data = (int32_t *)api_data;
    switch(data[0]) 
    {
        case 0: // Set GPIO to Input (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_INPUT);
        case 1: // Set GPIO to Output (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_OUTPUT);
        case 2: // Set GPIO to ALT0 (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_ALT0);
        case 3: // Set GPIO to ALT1 (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_ALT1);
        case 4: // Set GPIO to ALT2 (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_ALT2);
        case 5: // Set GPIO to ALT3 (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_ALT3);
        case 6: // Set GPIO to ALT4 (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_ALT4);
        case 7: // Set GPIO to ALT5 (data[1] = pin)
            return gpio_function(dev, data[1], GPIO_FUNCTION_ALT5);
        case 8: // Set GPIO Pin Pull (None)
            return gpio_pull(dev, data[1], GPIO_PULL_NONE);
        case 9: // Set GPIO Pin Pull (None)
            return gpio_pull(dev, data[1], GPIO_PULL_UP);
        case 10: // Set GPIO Pin Pull (None)
            return gpio_pull(dev, data[1], GPIO_PULL_DOWN);
        default:
            return -EOPNOTSUPP;
    }
    return 0;
}

static int init(device_t *dev, void *p)
{
    return 0;   // No actual initialization needed here!
}

DRIVER = {
    .compat = "gpio/bcm2711",
    .api = &api_call,
    .init = &init
};
