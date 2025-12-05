#include "keyboard_matrix.h"
#include "i2c_gpio_expander.h"
#include <stdio.h>

// Matrix state
static bool key_matrix_raw[MATRIX_ROWS][MATRIX_COLS] = {false};
static bool key_matrix_stable[MATRIX_ROWS][MATRIX_COLS] = {false};
static uint32_t key_debounce_time[MATRIX_ROWS][MATRIX_COLS] = {0};
static bool fake_input_enabled = false;
static bool fake_key_states[TOTAL_MATRIX_KEYS] = {false};

// GPIO assignments - UPDATE THESE FROM YOUR SCHEMATIC!
// Default: First 4 rows on Pico GPIO, last 4 on expanders
// Default: First 3 cols on Pico GPIO, last 3 on expanders
const uint matrix_row_pins[MATRIX_ROWS] = {
    6, 7, 8, 9,  // Rows 0-3: Pico GPIO
    10, 11, 12, 13  // Rows 4-7: Will be on expanders (update if different)
};

const uint matrix_col_pins[MATRIX_COLS] = {
    14, 15, 16,  // Cols 0-2: Pico GPIO
    17, 18, 19   // Cols 3-5: Will be on expanders (update if different)
};

// Expander assignments - UPDATE THESE!
// 0 = Pico GPIO, 1 = U2, 2 = U3
const uint8_t row_expander[MATRIX_ROWS] = {
    0, 0, 0, 0,  // First 4 rows on Pico
    1, 1, 1, 1   // Last 4 rows on U2 (update if different)
};

const uint8_t col_expander[MATRIX_COLS] = {
    0, 0, 0,     // First 3 cols on Pico
    1, 1, 1      // Last 3 cols on U2 (update if different)
};

// Expander pin mappings - UPDATE THESE!
// Which expander pin (0-15) corresponds to which matrix row/col
// Port A = pins 0-7, Port B = pins 8-15
const uint8_t row_expander_pin[MATRIX_ROWS] = {
    0, 0, 0, 0,  // Not used (Pico GPIO)
    0, 1, 2, 3   // U2 pins for rows 4-7 (update if different)
};

const uint8_t col_expander_pin[MATRIX_COLS] = {
    0, 0, 0,     // Not used (Pico GPIO)
    4, 5, 6      // U2 pins for cols 3-5 (update if different)
};

void keyboard_matrix_init(void) {
    // Initialize I2C expanders
    i2c_expander_init();
    
    // Initialize row pins as outputs (on Pico GPIO)
    for (int i = 0; i < MATRIX_ROWS; i++) {
        if (row_expander[i] == EXPANDER_NONE) {
            gpio_init(matrix_row_pins[i]);
            gpio_set_dir(matrix_row_pins[i], GPIO_OUT);
            gpio_put(matrix_row_pins[i], false);  // Start LOW
        } else {
            // Configure expander pin as output
            uint8_t addr = (row_expander[i] == EXPANDER_U2) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
            uint8_t port = (row_expander_pin[i] < 8) ? 0 : 1;
            uint8_t pin = row_expander_pin[i] % 8;
            
            // Set as output (0 = output)
            uint8_t direction;
            i2c_expander_read_port(addr, (port == 0) ? MCP23017_IODIRA : MCP23017_IODIRB, &direction);
            direction &= ~(1 << pin);  // Clear bit = output
            i2c_expander_set_direction(addr, port, direction);
            
            // Set LOW initially
            i2c_expander_write_pin(addr, port, pin, false);
        }
    }
    
    // Initialize column pins as inputs with pull-down (on Pico GPIO)
    for (int i = 0; i < MATRIX_COLS; i++) {
        if (col_expander[i] == EXPANDER_NONE) {
            gpio_init(matrix_col_pins[i]);
            gpio_set_dir(matrix_col_pins[i], GPIO_IN);
            gpio_pull_down(matrix_col_pins[i]);
        } else {
            // Configure expander pin as input with pull-up
            uint8_t addr = (col_expander[i] == EXPANDER_U2) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
            uint8_t port = (col_expander_pin[i] < 8) ? 0 : 1;
            uint8_t pin = col_expander_pin[i] % 8;
            
            // Set as input (1 = input)
            uint8_t direction;
            i2c_expander_read_port(addr, (port == 0) ? MCP23017_IODIRA : MCP23017_IODIRB, &direction);
            direction |= (1 << pin);  // Set bit = input
            i2c_expander_set_direction(addr, port, direction);
            
            // Enable pull-up
            uint8_t pullup;
            i2c_expander_read_port(addr, (port == 0) ? MCP23017_GPPUA : MCP23017_GPPUB, &pullup);
            pullup |= (1 << pin);
            i2c_expander_set_pullup(addr, port, pullup);
        }
    }
    
    printf("[MATRIX] Initialized %dx%d matrix (%d keys)\n", 
           MATRIX_ROWS, MATRIX_COLS, TOTAL_MATRIX_KEYS);
}

void keyboard_matrix_scan(void) {
    // If fake input is enabled, skip hardware scan
    if (fake_input_enabled) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                uint8_t key_index = matrix_to_key_index(row, col);
                key_matrix_stable[row][col] = fake_key_states[key_index];
            }
        }
        return;
    }
    
    uint32_t now = time_us_32();
    const uint32_t debounce_time_us = 5000;  // 5ms debounce
    
    for (int row = 0; row < MATRIX_ROWS; row++) {
        // Set current row HIGH
        if (row_expander[row] == EXPANDER_NONE) {
            gpio_put(matrix_row_pins[row], true);
        } else {
            uint8_t addr = (row_expander[row] == EXPANDER_U2) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
            uint8_t port = (row_expander_pin[row] < 8) ? 0 : 1;
            uint8_t pin = row_expander_pin[row] % 8;
            i2c_expander_write_pin(addr, port, pin, true);
        }
        
        // Small delay for settling
        sleep_us(10);
        
        // Read all columns
        for (int col = 0; col < MATRIX_COLS; col++) {
            bool pressed = false;
            
            if (col_expander[col] == EXPANDER_NONE) {
                // Read from Pico GPIO
                pressed = gpio_get(matrix_col_pins[col]);
            } else {
                // Read from expander
                uint8_t addr = (col_expander[col] == EXPANDER_U2) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
                uint8_t port = (col_expander_pin[col] < 8) ? 0 : 1;
                uint8_t pin = col_expander_pin[col] % 8;
                i2c_expander_read_pin(addr, port, pin, &pressed);
            }
            
            // Update raw state
            key_matrix_raw[row][col] = pressed;
            
            // Debounce
            if (pressed != key_matrix_stable[row][col]) {
                if ((now - key_debounce_time[row][col]) > debounce_time_us) {
                    key_matrix_stable[row][col] = pressed;
                    key_debounce_time[row][col] = now;
                }
            } else {
                key_debounce_time[row][col] = now;
            }
        }
        
        // Set current row LOW
        if (row_expander[row] == EXPANDER_NONE) {
            gpio_put(matrix_row_pins[row], false);
        } else {
            uint8_t addr = (row_expander[row] == EXPANDER_U2) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
            uint8_t port = (row_expander_pin[row] < 8) ? 0 : 1;
            uint8_t pin = row_expander_pin[row] % 8;
            i2c_expander_write_pin(addr, port, pin, false);
        }
    }
}

bool keyboard_matrix_get_key(uint8_t key_index) {
    if (key_index >= TOTAL_MATRIX_KEYS) return false;
    
    uint8_t row = key_index / MATRIX_COLS;
    uint8_t col = key_index % MATRIX_COLS;
    return keyboard_matrix_get_raw(row, col);
}

bool keyboard_matrix_get_raw(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        return false;
    }
    return key_matrix_stable[row][col];
}

uint8_t matrix_to_key_index(uint8_t row, uint8_t col) {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) {
        return 0xFF;  // Invalid
    }
    return row * MATRIX_COLS + col;
}

void keyboard_matrix_set_fake_key(uint8_t key_index, bool pressed) {
    if (key_index >= TOTAL_MATRIX_KEYS) return;
    fake_key_states[key_index] = pressed;
    fake_input_enabled = true;
}

