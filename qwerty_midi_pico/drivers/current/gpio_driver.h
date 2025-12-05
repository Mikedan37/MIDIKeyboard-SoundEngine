#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "pico/stdlib.h"

#define NUM_KEYS 24  // 24 columns (2 rows are for velocity detection, not separate keys)

void gpio_driver_init();
void gpio_poll_loop(); // Background polling loop for Core 1 - scans velocity matrix
bool gpio_is_pressed(uint8_t key_index); // Query key state by index (0-47)
uint8_t gpio_get_velocity(uint8_t key_index); // Get velocity for key (1-127)

#endif