#ifndef MSQT32_SHIFT_REGISTER_H
#define MSQT32_SHIFT_REGISTER_H

#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>

// MSQT32 Shift Register Configuration
// 24 columns total: 3 banks × 8 bits = 24 bits
#define MSQT32_BANKS 3
#define MSQT32_BITS_PER_BANK 8
#define MSQT32_TOTAL_BITS 24

// Pin assignments - UPDATE THESE FROM YOUR SCHEMATIC
#define SHIFT_DATA_PIN  10  // Data input pin
#define SHIFT_CLOCK_PIN 11  // Clock pin
#define SHIFT_LATCH_PIN  12 // Latch pin

// Function prototypes
void msqt32_init(void);
uint32_t msqt32_read_24bit(void);
void msqt32_shift_in_bit(bool bit);
void msqt32_pulse_latch(void);

#endif

