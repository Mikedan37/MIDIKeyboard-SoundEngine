#ifndef VELOCITY_MATRIX_H
#define VELOCITY_MATRIX_H

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>

// Matrix Configuration: 2 rows × 24 columns = 24 keys
// Rows are for velocity detection (early/late contact), not separate keys
#define MATRIX_ROWS 2
#define MATRIX_COLS 24
#define NUM_KEYS MATRIX_COLS  // 24 keys (one per column)

// Row pin assignments - UPDATE THESE FROM YOUR SCHEMATIC
#define ROW0_PIN 6   // Early contact (first touch)
#define ROW1_PIN 7   // Late contact (full press)

// Key state structure for velocity detection
typedef struct {
    bool isPressed;              // Current pressed state
    bool wasPressed;             // Previous frame state
    uint32_t firstContactTime;   // Timestamp when ROW0 detected (early)
    uint32_t secondContactTime;  // Timestamp when ROW1 detected (late)
    uint32_t releaseTime;        // Timestamp when key released
    uint8_t velocity;            // Computed velocity (1-127)
    bool velocityComputed;        // Whether velocity has been calculated
} key_state_t;

// Debouncing configuration
#define DEBOUNCE_TIME_US 5000    // 5ms debounce
#define VELOCITY_TIMEOUT_US 50000 // 50ms max time between contacts

// Velocity calculation
#define MIN_VELOCITY_TIME_US 100   // Fastest press = 100us = velocity 127
#define MAX_VELOCITY_TIME_US 50000 // Slowest press = 50ms = velocity 1

// Function prototypes
void velocity_matrix_init(void);
void velocity_matrix_scan(void);
bool velocity_matrix_get_key_pressed(uint8_t key_index);
uint8_t velocity_matrix_get_key_velocity(uint8_t key_index);
key_state_t* velocity_matrix_get_key_state(uint8_t key_index);
void velocity_matrix_reset_key(uint8_t key_index);

// Test helper: Expose calculate_velocity for testing
uint8_t calculate_velocity(uint32_t time_delta_us);

#endif

