#include "drivers/current/gpio_driver.h"
#include "pico/multicore.h"
#include "drivers/current/velocity_matrix.h"
#include <stdio.h>

static bool key_state[NUM_KEYS] = {false};
static bool prev_key_state[NUM_KEYS] = {false};
static uint8_t key_velocity[NUM_KEYS] = {0};

void gpio_driver_init() {
    velocity_matrix_init();
    printf("[GPIO] Driver initialized for %d velocity-sensitive keys\n", NUM_KEYS);
}

// Background polling loop for Core 1 - scans velocity-sensitive matrix
void gpio_poll_loop() {
    static uint32_t scan_counter = 0;
    
    while (true) {
        // Log matrix scan period (every 512 scans to avoid flooding)
        uint32_t scan_start = time_us_32();
        
        // Scan the velocity matrix (2-phase scanning)
        velocity_matrix_scan();
        
        uint32_t scan_end = time_us_32();
        uint32_t scan_period = scan_end - scan_start;
        
        // Log scan period every 512 scans (0x1FF = 511, so 0x200 = 512)
        if ((scan_counter++ & 0x1FF) == 0) {
            printf("SCAN_PERIOD_US,%lu\n", (unsigned long)scan_period);
        }
        
        // Update key states and velocities
        for (int i = 0; i < NUM_KEYS; i++) {
            bool current = velocity_matrix_get_key_pressed(i);
            uint8_t velocity = velocity_matrix_get_key_velocity(i);
            
            if (current && !prev_key_state[i]) {
                // Key pressed
                key_velocity[i] = velocity;
                printf("[GPIO] Note ON: %d, velocity=%d\n", i, velocity);
            } else if (!current && prev_key_state[i]) {
                // Key released
                printf("[GPIO] Note OFF: %d\n", i);
                key_velocity[i] = 0;
            }
            
            prev_key_state[i] = current;
            key_state[i] = current;
        }
        
        sleep_ms(2);  // Scan every 2ms (~500Hz scan rate)
    }
}

bool gpio_is_pressed(uint8_t key_index) {
    return (key_index < NUM_KEYS) ? key_state[key_index] : false;
}

uint8_t gpio_get_velocity(uint8_t key_index) {
    return (key_index < NUM_KEYS) ? key_velocity[key_index] : 0;
}