#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "pico/multicore.h"
#include "drivers/current/gpio_driver.h"
#include "drivers/current/velocity_matrix.h"  // For ROW0_PIN, ROW1_PIN constants

// MIDI and HID config for 24 keys (24 columns)
// MIDI notes: Chromatic scale starting at C4 (MIDI 60)
// Each column = one key = one MIDI note
const uint8_t midi_notes[NUM_KEYS] = {
    // 24 keys: C4 to B5 (chromatic)
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83
};

// HID keycodes: QWERTY layout mapping (24 keys)
// Update this based on your desired keyboard layout
const uint8_t hid_keycodes[NUM_KEYS] = {
    // Standard QWERTY layout (24 keys)
    0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,  // 1, 2, 3, 4, 5, 6
    0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C,  // Q, W, E, R, T, Y
    0x18, 0x0C, 0x12, 0x13, 0x2F, 0x2D,  // U, I, O, P, [, ]
    0x04, 0x16, 0x07, 0x09, 0x0A, 0x0B   // A, S, D, F, G, H
};

void send_midi_note(uint8_t note, bool on, uint8_t velocity) {
    uint8_t msg[] = { (on ? 0x90 : 0x80), note, velocity };
    tud_midi_stream_write(0, msg, 3);
}

void send_hid_key(uint8_t keycode) {
    if (!tud_hid_ready()) return;
    uint8_t report[6] = {keycode};
    tud_hid_keyboard_report(0, 0, report);
    sleep_ms(10);
    tud_hid_keyboard_report(0, 0, NULL);
}

int main() {
    board_init();
    tusb_init();
    gpio_driver_init();

    // Launch GPIO polling on Core 1
    multicore_launch_function(gpio_poll_loop, NULL, true);

    bool last_state[NUM_KEYS] = {false};
    uint8_t last_velocity[NUM_KEYS] = {0};

    printf("[MAIN] Starting velocity-sensitive keyboard (2×24 matrix, 24 keys)\n");
    printf("[MAIN] ROW0=GPIO%d (early contact), ROW1=GPIO%d (late contact)\n", ROW0_PIN, ROW1_PIN);

    while (true) {
        tud_task(); // USB tasks

        for (int i = 0; i < NUM_KEYS; ++i) {
            bool current = gpio_is_pressed(i);
            uint8_t velocity = gpio_get_velocity(i);

            if (current && !last_state[i]) {
                // Key pressed - send MIDI note ON with velocity
                send_midi_note(midi_notes[i], true, velocity);
                send_hid_key(hid_keycodes[i]);
                last_state[i] = true;
                last_velocity[i] = velocity;
                
                printf("[MAIN] Key %d: MIDI note %d, velocity %d\n", 
                       i, midi_notes[i], velocity);
            } else if (!current && last_state[i]) {
                // Key released - send MIDI note OFF
                send_midi_note(midi_notes[i], false, 0);
                last_state[i] = false;
                last_velocity[i] = 0;
            }
        }

        sleep_ms(2);
    }
}