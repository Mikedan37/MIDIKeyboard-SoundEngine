#ifndef KEY_MAP_H
#define KEY_MAP_H

#include <stdint.h>
#include <math.h>  // 👈 for pow()

#define NUM_KEYS 24

static const uint8_t gpio_key_pins[NUM_KEYS] = {
  2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13,
  14, 15, 16, 17, 18, 19,
  20, 21, 22, 26, 27, 28
};

static const uint8_t gpio_to_midi[NUM_KEYS] = {
  60, 61, 62, 63, 64, 65,
  66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77,
  78, 79, 80, 81, 82, 83
};

static const float midi_freq_table[NUM_KEYS] = {
  261.63, 277.18, 293.66, 311.13, 329.63, 349.23,
  369.99, 392.00, 415.30, 440.00, 466.16, 493.88,
  523.25, 554.37, 587.33, 622.25, 659.25, 698.46,
  739.99, 783.99, 830.61, 880.00, 932.33, 987.77
};

static inline float midi_note_to_freq(uint8_t note) {
    return 440.0 * pow(2.0, ((int)note - 69) / 12.0);
}

#endif // KEY_MAP_H