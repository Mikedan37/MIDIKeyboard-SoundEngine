#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "pico/multicore.h"
#include "drivers/current/gpio_driver.h"
#include "drivers/current/velocity_matrix.h"  // For ROW0_PIN, ROW1_PIN constants

// MIDI config for 25 keys (25 columns)
// MIDI notes: Chromatic scale starting at C4 (MIDI 60)
// Each column = one key = one MIDI note
const uint8_t midi_notes[NUM_KEYS] = {
    // 25 keys: C4 to C6 (chromatic)
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84
};

void send_midi_note(uint8_t note, bool on, uint8_t velocity) {
    uint8_t msg[] = { (on ? 0x90 : 0x80), note, velocity };
    tud_midi_stream_write(0, msg, 3);
}


int main() {
    board_init();
    tusb_init();
    gpio_driver_init();

    // Launch GPIO polling on Core 1
    multicore_launch_function(gpio_poll_loop, NULL, true);

    bool last_state[NUM_KEYS] = {false};
    uint8_t last_velocity[NUM_KEYS] = {0};

    printf("[MAIN] Starting velocity-sensitive keyboard (2×25 matrix, 25 keys)\n");
    printf("[MAIN] ROW0=GPIO%d (early contact), ROW1=GPIO%d (late contact)\n", ROW0_PIN, ROW1_PIN);

    while (true) {
        tud_task(); // USB tasks

        for (int i = 0; i < NUM_KEYS; ++i) {
            bool current = gpio_is_pressed(i);
            uint8_t velocity = gpio_get_velocity(i);

            if (current && !last_state[i]) {
                // Key pressed - log event detection timestamp
                uint32_t t_event = time_us_32();
                printf("NOTE_EVENT,note=%u,vel=%u,t_us=%lu\n",
                       midi_notes[i], velocity, (unsigned long)t_event);
                
                // Send MIDI note ON with velocity
                send_midi_note(midi_notes[i], true, velocity);
                
                // Log USB send timestamp
                uint32_t t_send = time_us_32();
                printf("NOTE_SEND,note=%u,vel=%u,t_us=%lu\n",
                       midi_notes[i], velocity, (unsigned long)t_send);
                
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