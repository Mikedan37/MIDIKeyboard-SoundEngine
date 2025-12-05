#include "key_map.h"
#include "tonegen.h"
#include "sim_hardware.h"
#include "gpio_driver.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

void scan_keys_and_play() {
    for (int i = 0; i < NUM_KEYS; ++i) {
        if (gpio_read(gpio_key_pins[i])) {
            float freq = midi_freq_table[i];
            printf("🎹 GPIO %d active → Playing %.2f Hz\n", gpio_key_pins[i], freq);
            start_note(freq);
        }
    }
}

void send_keypress(uint8_t keycode) {
    if (!tud_hid_ready()) return;

    uint8_t report[6] = {0};
    report[0] = keycode;

    tud_hid_keyboard_report(0, 0, report);
    tud_hid_keyboard_report(0, 0, NULL); // release
}

void fake_button_input() {
    static uint32_t last_sent = 0;
    static int key_index = 0;
    uint32_t now = board_millis();

    if ((now - last_sent) >= 1000) {
        float freq = midi_freq_table[key_index];
        printf("🔊 Playing tone: %.2f Hz\n", freq);
        start_note(freq);

        key_index = (key_index + 1) % NUM_KEYS;
        last_sent = now;
    }
}


int main() {
    printf("👻 SIM START\n");
    tonegen_init();
    gpio_driver_init();
    while (1) {
        scan_keys_and_play();
        usleep(100000); // 100ms scan delay
    }
}