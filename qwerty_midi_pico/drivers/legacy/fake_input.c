#include "fake_input.h"
#include "keyboard_matrix.h"
#include <stdio.h>
#include <stdlib.h>

static bool fake_input_active = false;
static uint32_t last_fake_time = 0;
static uint32_t fake_sequence_index = 0;

// Test sequence: Play a simple scale
static const uint8_t test_sequence[] = {
    0, 2, 4, 5, 7, 9, 11, 12,  // C major scale
    14, 16, 18, 19, 21, 23, 25, 26,  // Next octave
    0xFF  // End marker
};

void fake_input_init(void) {
    #if ENABLE_FAKE_INPUT
    fake_input_active = true;
    printf("[FAKE] Fake input enabled for testing\n");
    #else
    fake_input_active = false;
    #endif
}

void fake_input_enable(bool enable) {
    fake_input_active = enable;
    printf("[FAKE] Fake input %s\n", enable ? "enabled" : "disabled");
}

void fake_input_update(void) {
    if (!fake_input_active) return;
    
    uint32_t now = time_us_32();
    
    // Simulate key presses in sequence every 500ms
    if ((now - last_fake_time) > 500000) {
        uint8_t key = test_sequence[fake_sequence_index];
        
        if (key == 0xFF) {
            // End of sequence, restart
            fake_sequence_index = 0;
            key = test_sequence[0];
        }
        
        // Press key
        fake_input_simulate_key(key, true);
        
        // Release after 200ms
        sleep_ms(200);
        fake_input_simulate_key(key, false);
        
        fake_sequence_index++;
        if (test_sequence[fake_sequence_index] == 0xFF) {
            fake_sequence_index = 0;
        }
        
        last_fake_time = now;
    }
}

void fake_input_simulate_key(uint8_t key_index, bool pressed) {
    if (key_index >= NUM_KEYS) return;
    
    keyboard_matrix_set_fake_key(key_index, pressed);
    
    printf("[FAKE] Key %d %s\n", key_index, pressed ? "PRESSED" : "RELEASED");
}

