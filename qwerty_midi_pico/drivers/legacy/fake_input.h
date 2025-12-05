#ifndef FAKE_INPUT_H
#define FAKE_INPUT_H

#include "pico/stdlib.h"

// Enable fake input for testing without hardware
// Set to 1 to enable, 0 to disable
#define ENABLE_FAKE_INPUT 1

void fake_input_init(void);
void fake_input_update(void);  // Call this in main loop
void fake_input_simulate_key(uint8_t key_index, bool pressed);
void fake_input_enable(bool enable);

#endif

