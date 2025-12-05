#ifndef KEYBOARD_MATRIX_H
#define KEYBOARD_MATRIX_H

#include "pico/stdlib.h"

// Matrix configuration - 48 keys total
// Common configurations: 8x6, 6x8, 12x4
// Adjust based on your schematic
#define MATRIX_ROWS 8
#define MATRIX_COLS 6
#define TOTAL_MATRIX_KEYS (MATRIX_ROWS * MATRIX_COLS)  // 48 keys

// Expander types
#define EXPANDER_NONE 0  // Pico GPIO
#define EXPANDER_U2   1  // First expander
#define EXPANDER_U3   2  // Second expander

// Row/Column GPIO assignments - UPDATE THESE FROM YOUR SCHEMATIC!
// These are examples - replace with actual GPIO pins from your design
extern const uint matrix_row_pins[MATRIX_ROWS];
extern const uint matrix_col_pins[MATRIX_COLS];

// Which expander handles which rows/cols
// 0 = Pico GPIO, 1 = U2, 2 = U3
extern const uint8_t row_expander[MATRIX_ROWS];
extern const uint8_t col_expander[MATRIX_COLS];

// Expander pin assignments (which expander pin = which matrix row/col)
// For rows on expanders: which expander pin (0-15) corresponds to matrix row
// For cols on expanders: which expander pin (0-15) corresponds to matrix col
extern const uint8_t row_expander_pin[MATRIX_ROWS];
extern const uint8_t col_expander_pin[MATRIX_COLS];

// Function prototypes
void keyboard_matrix_init(void);
void keyboard_matrix_scan(void);
bool keyboard_matrix_get_key(uint8_t key_index);
bool keyboard_matrix_get_raw(uint8_t row, uint8_t col);
uint8_t matrix_to_key_index(uint8_t row, uint8_t col);
void keyboard_matrix_set_fake_key(uint8_t key_index, bool pressed);  // For simulation

#endif

