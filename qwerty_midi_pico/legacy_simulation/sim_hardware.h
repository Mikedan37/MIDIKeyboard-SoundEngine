#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static const uint8_t test_keys[NUM_KEYS] = {0x04, 0x05, 0x06, 0x07}; // A B C D

static inline uint32_t board_millis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static inline int tud_hid_ready() {
    return 1;
}

static inline void tud_hid_keyboard_report(uint8_t id, uint8_t modifier, uint8_t const keycode[6]) {
    if (keycode)
        printf("SIMULATED: Sent keycode 0x%02X\n", keycode[0]);
    else
        printf("SIMULATED: Key released\n");
}

// Simulated GPIO read function
static inline int gpio_read(uint8_t pin) {
    // For simulation, return random or test values
    // In a real sim, this would read from simulated hardware
    return 0; // Default to not pressed
}