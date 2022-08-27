/*
 * (c) 2022 Apollo Project Developers
 * For terms, see LICENSE
 * i8254.c - Driver for the 8253/8254 Programmable Interrupt Timer
 */

#include <sys/resource.h>
#include <sys/timekeeping.h>
#include <sys/device.h>
#include <stdint.h>
#include "include/i8254.h"

int i8254_configure(timekeeping_state_t *chip, int channel, int mode)
{
    uint32_t divisor;
    uint8_t low, high, init_byte;
    uint8_t channel_select = 0;
    int ret;

    switch(channel)
    {
        case 0:
            channel_select = i8254_DATA_0;    
            init_byte = i8254_CHANNEL_0;
            break;
        case 1:
            channel_select = i8254_DATA_1;
            init_byte = i8254_CHANNEL_1;
            break;
        case 2:
            channel_select = i8254_DATA_2;
            init_byte = i8254_CHANNEL_2;
            break;
        default:
            return -1;
    }

    switch(mode)
    {
        case 0: 
            init_byte |= i8254_MODE_0;
            break;
        case 1: 
            init_byte |= i8254_MODE_1;
            break; 
        case 2: 
            init_byte |= i8254_MODE_2;
            break; 
        case 3: 
            init_byte |= i8254_MODE_3;
            break; 
        case 4: 
            init_byte |= i8254_MODE_4;
            break; 
        case 5: 
            init_byte |= i8254_MODE_5;
            break; 
        default:
            return -1;
    }

    init_byte |= i8254_LHBYTE | i8254_BIN_MODE;
    divisor    = chip->base_frequency / chip->frequency;
    low        = (uint8_t)(divisor & 0xFF);
    high       = (uint8_t)((divisor >> 8) & 0xFF);

    ret  = resource_write(&init_byte, chip->data, i8254_CMD, 1);
    ret |= resource_write(&low, chip->data, channel_select, 1);
    ret |= resource_write(&high, chip->data, channel_select, 1);

    return ret;
}

static int configure(device_t *device, void *p)
{
    int *config = (int *)p;
    timekeeping_state_t *chip = (timekeeping_state_t *)device->data;
    return i8254_configure(chip, config[0], config[1]);
}

DRIVER = {
    .compat = "clock/i8254",
    .init   = &configure
};