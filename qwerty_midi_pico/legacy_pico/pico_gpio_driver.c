#include "gpio_driver.h"
#include "pico/stdlib.h"

void gpio_init() {
    for (int i = 0; i < NUM_KEYS; ++i) {
        gpio_init(gpio_key_pins[i]);
        gpio_set_dir(gpio_key_pins[i], GPIO_IN);
        gpio_pull_down(gpio_key_pins[i]);
    }
}

int gpio_read(uint8_t pin) {
    return gpio_get(pin);
}