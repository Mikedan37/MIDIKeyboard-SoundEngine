#include "drivers/current/velocity_matrix.h"
#include "drivers/current/msqt32_shift_register.h"
#include <stdio.h>
#include <string.h>

// Per-key state array (24 keys)
static key_state_t key_states[NUM_KEYS];

// Current column states
static uint32_t row0_columns = 0;  // Early contact state
static uint32_t row1_columns = 0;  // Late contact state

// Calculate velocity function (exposed for testing)
uint8_t calculate_velocity(uint32_t time_delta_us) {
    // Calculate velocity based on time between ROW0 and ROW1 contacts
    // Faster press = higher velocity (shorter time = higher number)
    // Velocity range: 1-127 (MIDI standard)
    
    if (time_delta_us < MIN_VELOCITY_TIME_US) {
        return 127;  // Maximum velocity
    }
    
    if (time_delta_us > MAX_VELOCITY_TIME_US) {
        return 1;  // Minimum velocity
    }
    
    // Linear interpolation: time_delta_us → velocity (127-1)
    // Inverse relationship: shorter time = higher velocity
    uint32_t time_range = MAX_VELOCITY_TIME_US - MIN_VELOCITY_TIME_US;
    uint32_t velocity_range = 126;  // 127 - 1
    
    uint32_t normalized_time = time_delta_us - MIN_VELOCITY_TIME_US;
    uint8_t velocity = 127 - ((normalized_time * velocity_range) / time_range);
    
    // Ensure velocity is in valid range
    if (velocity < 1) velocity = 1;
    if (velocity > 127) velocity = 127;
    
    return velocity;
}

void velocity_matrix_init(void) {
    // Initialize MSQT32 shift register interface
    msqt32_init();
    
    // Initialize row pins as outputs
    gpio_init(ROW0_PIN);
    gpio_set_dir(ROW0_PIN, GPIO_OUT);
    gpio_put(ROW0_PIN, false);
    
    gpio_init(ROW1_PIN);
    gpio_set_dir(ROW1_PIN, GPIO_OUT);
    gpio_put(ROW1_PIN, false);
    
    // Initialize all key states
    memset(key_states, 0, sizeof(key_states));
    
    printf("[VELOCITY] Initialized 2×24 velocity-sensitive matrix\n");
    printf("[VELOCITY] ROW0 (early)=GPIO%d, ROW1 (late)=GPIO%d\n", ROW0_PIN, ROW1_PIN);
}

void velocity_matrix_scan(void) {
    uint32_t now = time_us_32();
    
    // ============================================================
    // SCAN CYCLE: Two-phase scanning for velocity detection
    // ============================================================
    
    // PHASE 1: Drive ROW0 HIGH, ROW1 LOW
    // This detects the "early contact" (first touch)
    gpio_put(ROW0_PIN, true);
    gpio_put(ROW1_PIN, false);
    sleep_us(10);  // Settling time
    
    // Read all 24 columns via MSQT32 shift registers
    row0_columns = msqt32_read_24bit();
    
    // PHASE 2: Drive ROW0 LOW, ROW1 HIGH
    // This detects the "late contact" (full press)
    gpio_put(ROW0_PIN, false);
    gpio_put(ROW1_PIN, true);
    sleep_us(10);  // Settling time
    
    // Read all 24 columns again
    row1_columns = msqt32_read_24bit();
    
    // Reset rows to idle state
    gpio_put(ROW0_PIN, false);
    gpio_put(ROW1_PIN, false);
    
    // ============================================================
    // PROCESS EACH KEY: Detect presses and calculate velocity
    // ============================================================
    // Note: Each column represents ONE key
    // ROW0 = early contact (first touch)
    // ROW1 = late contact (full press)
    // Velocity = time difference between ROW0 and ROW1
    
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        uint8_t key_index = col;  // One key per column (0-23)
        
        // Check if column bit is set in either row
        bool row0_pressed = (row0_columns >> col) & 1;  // Early contact
        bool row1_pressed = (row1_columns >> col) & 1;  // Late contact
        bool key_pressed = row0_pressed || row1_pressed;
        
        key_state_t *key = &key_states[key_index];
        
        // ============================================================
        // KEY PRESS DETECTION
        // ============================================================
        
        if (key_pressed && !key->wasPressed) {
            // NEW KEY PRESS DETECTED
            
            // Record first contact time (ROW0 = early contact)
            if (row0_pressed && key->firstContactTime == 0) {
                key->firstContactTime = now;
            }
            
            // Record second contact time (ROW1 = late contact)
            if (row1_pressed && key->secondContactTime == 0) {
                key->secondContactTime = now;
                
                // Calculate velocity if both contacts detected
                if (key->firstContactTime > 0) {
                    uint32_t time_delta = key->secondContactTime - key->firstContactTime;
                    
                    // Only calculate if within valid time window
                    if (time_delta < VELOCITY_TIMEOUT_US) {
                        key->velocity = calculate_velocity(time_delta);
                        key->velocityComputed = true;
                    } else {
                        // Timeout - use default velocity
                        key->velocity = 64;  // Medium velocity
                        key->velocityComputed = true;
                    }
                } else {
                    // Only ROW1 detected (very fast press, ROW0 missed)
                    // Use maximum velocity
                    key->velocity = 127;
                    key->velocityComputed = true;
                }
            }
            
            // Mark as pressed (with debouncing)
            if ((now - key->releaseTime) > DEBOUNCE_TIME_US) {
                key->isPressed = true;
            }
        }
        
        // ============================================================
        // KEY RELEASE DETECTION
        // ============================================================
        
        if (!key_pressed && key->wasPressed) {
            // KEY RELEASED
            
            // Reset velocity calculation state
            key->firstContactTime = 0;
            key->secondContactTime = 0;
            key->velocityComputed = false;
            key->releaseTime = now;
            
            // Mark as released (with debouncing)
            if ((now - key->releaseTime) > DEBOUNCE_TIME_US) {
                key->isPressed = false;
            }
        }
        
        // Update previous state
        key->wasPressed = key_pressed;
    }
}

bool velocity_matrix_get_key_pressed(uint8_t key_index) {
    if (key_index >= NUM_KEYS) return false;
    return key_states[key_index].isPressed;
}

uint8_t velocity_matrix_get_key_velocity(uint8_t key_index) {
    if (key_index >= NUM_KEYS) return 0;
    
    key_state_t *key = &key_states[key_index];
    
    if (key->velocityComputed) {
        return key->velocity;
    }
    
    // Default velocity if not yet computed
    return 64;  // Medium velocity
}

key_state_t* velocity_matrix_get_key_state(uint8_t key_index) {
    if (key_index >= NUM_KEYS) return NULL;
    return &key_states[key_index];
}

void velocity_matrix_reset_key(uint8_t key_index) {
    if (key_index >= NUM_KEYS) return;
    
    key_state_t *key = &key_states[key_index];
    memset(key, 0, sizeof(key_state_t));
}

