#include "drivers/current/msqt32_shift_register.h"
#include <stdio.h>

void msqt32_init(void) {
    // Configure shift register pins
    gpio_init(SHIFT_DATA_PIN);
    gpio_set_dir(SHIFT_DATA_PIN, GPIO_IN);  // Data input from shift register
    
    gpio_init(SHIFT_CLOCK_PIN);
    gpio_set_dir(SHIFT_CLOCK_PIN, GPIO_OUT);
    gpio_put(SHIFT_CLOCK_PIN, false);
    
    gpio_init(SHIFT_LATCH_PIN);
    gpio_set_dir(SHIFT_LATCH_PIN, GPIO_OUT);
    gpio_put(SHIFT_LATCH_PIN, false);
    
    printf("[MSQT32] Initialized shift register interface\n");
    printf("[MSQT32] DATA=%d, CLOCK=%d, LATCH=%d\n", 
           SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, SHIFT_LATCH_PIN);
}

void msqt32_pulse_latch(void) {
    // Latch the current state of shift registers
    gpio_put(SHIFT_LATCH_PIN, true);
    sleep_us(1);  // Minimum latch pulse width
    gpio_put(SHIFT_LATCH_PIN, false);
}

bool msqt32_shift_in_bit(void) {
    // Read one bit from shift register
    bool bit = gpio_get(SHIFT_DATA_PIN);
    
    // Pulse clock to shift next bit
    gpio_put(SHIFT_CLOCK_PIN, true);
    sleep_us(1);  // Clock pulse width
    gpio_put(SHIFT_CLOCK_PIN, false);
    sleep_us(1);  // Setup time
    
    return bit;
}

uint32_t msqt32_read_24bit(void) {
    // Latch current state
    msqt32_pulse_latch();
    
    // Read 24 bits (3 banks × 8 bits)
    uint32_t data = 0;
    
    for (int i = 0; i < MSQT32_TOTAL_BITS; i++) {
        bool bit = msqt32_shift_in_bit();
        if (bit) {
            data |= (1UL << i);
        }
    }
    
    return data;
}

