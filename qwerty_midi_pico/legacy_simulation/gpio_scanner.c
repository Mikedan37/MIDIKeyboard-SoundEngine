// File: qwerty_midi_pico/sim/gpio_scanner.c
// Description: Scans all 24 GPIO pins, supports multi-key detection + debouncing

#include "key_map.h"
#include "tonegen.h"
#include "gpio_driver.h"
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define DEBOUNCE_MS 50
#define MAX_KEYS 24

static int last_state[MAX_KEYS] = {0};
static uint32_t last_change_time[MAX_KEYS] = {0};

uint32_t millis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void scan_keys_and_play() {
    uint32_t now = millis();

    for (int i = 0; i < NUM_KEYS; i++) {
        int pin = gpio_key_pins[i];
        int val = gpio_read(pin);

        if (val != last_state[i]) {
            if (now - last_change_time[i] > DEBOUNCE_MS) {
                last_change_time[i] = now;
                last_state[i] = val;

                if (val) {
                    float freq = midi_freq_table[i];
                    printf("🎹 GPIO %d active → %.2f Hz\n", pin, freq);
                    start_note(freq);
                }
            }
        }
    }
}
