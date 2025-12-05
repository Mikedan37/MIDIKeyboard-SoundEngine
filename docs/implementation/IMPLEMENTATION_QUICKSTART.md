# Quick Start: Implementing Keyboard Matrix + I2C

## Step-by-Step Implementation Guide

### Step 1: Create I2C GPIO Expander Driver

**File**: `qwerty_midi_pico/drivers/i2c_gpio_expander.h`
```c
#ifndef I2C_GPIO_EXPANDER_H
#define I2C_GPIO_EXPANDER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// MCP23017 I2C addresses (adjust based on your hardware)
#define EXPANDER_U2_ADDR 0x20 // A0=0, A1=0, A2=0
#define EXPANDER_U3_ADDR 0x21 // A0=1, A1=0, A2=0

// MCP23017 Register addresses
#define MCP23017_IODIRA 0x00 // I/O Direction Register A
#define MCP23017_IODIRB 0x01 // I/O Direction Register B
#define MCP23017_GPIOA 0x12 // GPIO Register A
#define MCP23017_GPIOB 0x13 // GPIO Register B
#define MCP23017_GPPUA 0x0C // Pull-up Register A
#define MCP23017_GPPUB 0x0D // Pull-up Register B

// I2C Configuration
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4 // GPIO4
#define I2C_SCL_PIN 5 // GPIO5
#define I2C_FREQ 400000 // 400kHz

// Function prototypes
void i2c_expander_init(void);
bool i2c_expander_read_port(uint8_t addr, uint8_t port, uint8_t *data);
bool i2c_expander_write_port(uint8_t addr, uint8_t port, uint8_t data);
bool i2c_expander_set_direction(uint8_t addr, uint8_t port, uint8_t direction);
bool i2c_expander_set_pullup(uint8_t addr, uint8_t port, uint8_t pullup);

#endif
```

**File**: `qwerty_midi_pico/drivers/i2c_gpio_expander.c`
```c
#include "i2c_gpio_expander.h"
#include <stdio.h>

void i2c_expander_init(void) {
 // Initialize I2C
 i2c_init(I2C_PORT, I2C_FREQ);

 // Set GPIO functions
 gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
 gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

 // Enable pull-ups
 gpio_pull_up(I2C_SDA_PIN);
 gpio_pull_up(I2C_SCL_PIN);

 // Configure both expanders
 // Set all pins as inputs initially
 i2c_expander_set_direction(EXPANDER_U2_ADDR, 0, 0xFF); // Port A = inputs
 i2c_expander_set_direction(EXPANDER_U2_ADDR, 1, 0xFF); // Port B = inputs
 i2c_expander_set_direction(EXPANDER_U3_ADDR, 0, 0xFF);
 i2c_expander_set_direction(EXPANDER_U3_ADDR, 1, 0xFF);

 // Enable pull-ups
 i2c_expander_set_pullup(EXPANDER_U2_ADDR, 0, 0xFF);
 i2c_expander_set_pullup(EXPANDER_U2_ADDR, 1, 0xFF);
 i2c_expander_set_pullup(EXPANDER_U3_ADDR, 0, 0xFF);
 i2c_expander_set_pullup(EXPANDER_U3_ADDR, 1, 0xFF);
}

bool i2c_expander_write_port(uint8_t addr, uint8_t port, uint8_t data) {
 uint8_t buf[2];
 buf[0] = port;
 buf[1] = data;

 int ret = i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
 return ret == 2;
}

bool i2c_expander_read_port(uint8_t addr, uint8_t port, uint8_t *data) {
 uint8_t reg = port;

 // Write register address
 if (i2c_write_blocking(I2C_PORT, addr, &reg, 1, true) != 1) {
 return false;
 }

 // Read data
 if (i2c_read_blocking(I2C_PORT, addr, data, 1, false) != 1) {
 return false;
 }

 return true;
}

bool i2c_expander_set_direction(uint8_t addr, uint8_t port, uint8_t direction) {
 // 0 = output, 1 = input
 uint8_t reg = (port == 0) ? MCP23017_IODIRA : MCP23017_IODIRB;
 return i2c_expander_write_port(addr, reg, direction);
}

bool i2c_expander_set_pullup(uint8_t addr, uint8_t port, uint8_t pullup) {
 uint8_t reg = (port == 0) ? MCP23017_GPPUA : MCP23017_GPPUB;
 return i2c_expander_write_port(addr, reg, pullup);
}
```

---

### Step 2: Create Keyboard Matrix Driver

**File**: `qwerty_midi_pico/drivers/keyboard_matrix.h`
```c
#ifndef KEYBOARD_MATRIX_H
#define KEYBOARD_MATRIX_H

#include "pico/stdlib.h"

// Matrix configuration - ADJUST BASED ON YOUR SCHEMATIC
#define MATRIX_ROWS 8
#define MATRIX_COLS 6
#define TOTAL_MATRIX_KEYS (MATRIX_ROWS * MATRIX_COLS) // 48 keys

// Row/Column GPIO assignments - NEED TO DETERMINE FROM SCHEMATIC
// These are examples - replace with actual GPIO pins from your design
extern const uint matrix_row_pins[MATRIX_ROWS];
extern const uint matrix_col_pins[MATRIX_COLS];

// Which expander handles which rows/cols - NEED TO DETERMINE
// 0 = Pico GPIO, 1 = U2, 2 = U3
extern const uint8_t row_expander[MATRIX_ROWS];
extern const uint8_t col_expander[MATRIX_COLS];

// Function prototypes
void keyboard_matrix_init(void);
void keyboard_matrix_scan(void);
bool keyboard_matrix_get_key(uint8_t key_index);
bool keyboard_matrix_get_raw(uint8_t row, uint8_t col);
uint8_t matrix_to_key_index(uint8_t row, uint8_t col);

#endif
```

**File**: `qwerty_midi_pico/drivers/keyboard_matrix.c`
```c
#include "keyboard_matrix.h"
#include "i2c_gpio_expander.h"
#include <stdio.h>

// Matrix state
static bool key_matrix_raw[MATRIX_ROWS][MATRIX_COLS] = {false};
static bool key_matrix_stable[MATRIX_ROWS][MATRIX_COLS] = {false};
static uint32_t key_debounce_time[MATRIX_ROWS][MATRIX_COLS] = {0};

// GPIO assignments - UPDATE THESE FROM YOUR SCHEMATIC!
const uint matrix_row_pins[MATRIX_ROWS] = {
 // Example - replace with actual GPIO pins
 6, 7, 8, 9, 10, 11, 12, 13
};

const uint matrix_col_pins[MATRIX_COLS] = {
 // Example - replace with actual GPIO pins
 14, 15, 16, 17, 18, 19
};

// Expander assignments - UPDATE THESE!
// 0 = Pico GPIO, 1 = U2, 2 = U3
const uint8_t row_expander[MATRIX_ROWS] = {
 0, 0, 0, 0, 1, 1, 1, 1 // First 4 rows on Pico, last 4 on U2
};

const uint8_t col_expander[MATRIX_COLS] = {
 0, 0, 1, 1, 2, 2 // Mixed between Pico and expanders
};

void keyboard_matrix_init(void) {
 // Initialize row pins as outputs (on Pico GPIO)
 for (int i = 0; i < MATRIX_ROWS; i++) {
 if (row_expander[i] == 0) {
 gpio_init(matrix_row_pins[i]);
 gpio_set_dir(matrix_row_pins[i], GPIO_OUT);
 gpio_put(matrix_row_pins[i], false); // Start LOW
 }
 }

 // Initialize column pins as inputs with pull-down (on Pico GPIO)
 for (int i = 0; i < MATRIX_COLS; i++) {
 if (col_expander[i] == 0) {
 gpio_init(matrix_col_pins[i]);
 gpio_set_dir(matrix_col_pins[i], GPIO_IN);
 gpio_pull_down(matrix_col_pins[i]);
 }
 }

 // Configure expander pins
 // Rows on expanders should be outputs
 // Columns on expanders should be inputs with pull-ups
 // (Implementation depends on which expander pins are used)
}

void keyboard_matrix_scan(void) {
 uint32_t now = time_us_32();
 const uint32_t debounce_time_us = 5000; // 5ms debounce

 for (int row = 0; row < MATRIX_ROWS; row++) {
 // Set current row HIGH
 if (row_expander[row] == 0) {
 gpio_put(matrix_row_pins[row], true);
 } else {
 // Set expander pin HIGH
 uint8_t expander_addr = (row_expander[row] == 1) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
 // Need to determine which expander pin corresponds to this row
 // i2c_expander_write_port(expander_addr, MCP23017_GPIOA, ...);
 }

 // Small delay for settling
 sleep_us(10);

 // Read all columns
 for (int col = 0; col < MATRIX_COLS; col++) {
 bool pressed = false;

 if (col_expander[col] == 0) {
 // Read from Pico GPIO
 pressed = gpio_get(matrix_col_pins[col]);
 } else {
 // Read from expander
 uint8_t expander_addr = (col_expander[col] == 1) ? EXPANDER_U2_ADDR : EXPANDER_U3_ADDR;
 uint8_t port_data;
 // Need to determine which expander pin/port
 if (i2c_expander_read_port(expander_addr, MCP23017_GPIOA, &port_data)) {
 // Extract specific bit for this column
 // pressed = (port_data >> col_bit) & 1;
 }
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
 if (row_expander[row] == 0) {
 gpio_put(matrix_row_pins[row], false);
 } else {
 // Set expander pin LOW
 // i2c_expander_write_port(...);
 }
 }
}

bool keyboard_matrix_get_key(uint8_t key_index) {
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
 return row * MATRIX_COLS + col;
}
```

---

### Step 3: Update GPIO Driver to Use Matrix

**File**: `qwerty_midi_pico/drivers/gpio_driver.h`
```c
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "pico/stdlib.h"

#define NUM_KEYS 48 // Changed from 5

void gpio_driver_init();
void gpio_poll_loop(); // Scans matrix on Core 1
bool gpio_is_pressed(uint8_t key_index); // Query key state

#endif
```

**File**: `qwerty_midi_pico/drivers/gpio_driver.c`
```c
#include "gpio_driver.h"
#include "pico/multicore.h"
#include "keyboard_matrix.h"
#include <stdio.h>

static bool key_state[NUM_KEYS] = {false};
static bool prev_key_state[NUM_KEYS] = {false};

void gpio_driver_init() {
 keyboard_matrix_init();
}

// Background polling loop for Core 1
void gpio_poll_loop() {
 while (true) {
 // Scan the matrix
 keyboard_matrix_scan();

 // Update key states
 for (int i = 0; i < NUM_KEYS; i++) {
 bool current = keyboard_matrix_get_key(i);

 if (current && !prev_key_state[i]) {
 printf("Note ON: %d\n", i);
 } else if (!current && prev_key_state[i]) {
 printf("Note OFF: %d\n", i);
 }

 prev_key_state[i] = current;
 key_state[i] = current;
 }

 sleep_ms(2); // Scan every 2ms
 }
}

bool gpio_is_pressed(uint8_t key_index) {
 return (key_index < NUM_KEYS) ? key_state[key_index] : false;
}
```

---

### Step 4: Update Main.c Key Mappings

**File**: `qwerty_midi_pico/main.c`
```c
// ... existing includes ...

// 48 keys - UPDATE WITH YOUR DESIRED MAPPING
const uint8_t midi_notes[NUM_KEYS] = {
 // Example: Chromatic scale starting at C4 (MIDI 60)
 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, // C4-B4
 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, // C5-B5
 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, // C6-B6
 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107 // C7-B7
};

// HID keycodes - UPDATE WITH YOUR DESIRED MAPPING
const uint8_t hid_keycodes[NUM_KEYS] = {
 // Example: QWERTY layout
 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
 // ... (fill in all 48 keycodes)
};

// ... rest of main.c stays the same ...
```

---

### Step 5: Add Fake Input Support

**File**: `qwerty_midi_pico/drivers/fake_input.h`
```c
#ifndef FAKE_INPUT_H
#define FAKE_INPUT_H

#include "pico/stdlib.h"

void fake_input_init(void);
void fake_input_update(void); // Call this in main loop
void fake_input_simulate_key(uint8_t key_index, bool pressed);

#endif
```

**File**: `qwerty_midi_pico/drivers/fake_input.c`
```c
#include "fake_input.h"
#include "keyboard_matrix.h"
#include <stdio.h>
#include <stdlib.h>

static bool fake_input_enabled = false;
static uint32_t last_fake_time = 0;

void fake_input_init(void) {
 // Enable via compile flag or runtime check
 #ifdef ENABLE_FAKE_INPUT
 fake_input_enabled = true;
 #endif
}

void fake_input_update(void) {
 if (!fake_input_enabled) return;

 uint32_t now = time_us_32();

 // Simulate random key presses every 2 seconds
 if ((now - last_fake_time) > 2000000) {
 uint8_t random_key = rand() % NUM_KEYS;
 fake_input_simulate_key(random_key, true);
 sleep_ms(100);
 fake_input_simulate_key(random_key, false);
 last_fake_time = now;
 }
}

void fake_input_simulate_key(uint8_t key_index, bool pressed) {
 // Directly set matrix state (bypasses hardware)
 uint8_t row = key_index / MATRIX_COLS;
 uint8_t col = key_index % MATRIX_COLS;

 // This would need to be integrated with keyboard_matrix.c
 // to directly set the stable state
 printf("[FAKE] Key %d (%d,%d) %s\n", key_index, row, col, 
 pressed ? "PRESSED" : "RELEASED");
}
```

---

### Step 6: Update CMakeLists.txt

**File**: `qwerty_midi_pico/CMakeLists.txt`
```cmake
add_executable(qwerty_midi_pico
 main.c
 drivers/gpio_driver.c
 drivers/i2c_gpio_expander.c # NEW
 drivers/keyboard_matrix.c # NEW
 drivers/fake_input.c # NEW (optional)
 usb_descriptors.c
)
```

---

## Critical Information Needed from Schematic

To complete the implementation, you need to determine:

1. **Matrix Configuration:**
 - How many rows? (likely 6-8)
 - How many columns? (likely 6-8)
 - Total should equal 48 keys

2. **GPIO Pin Assignments:**
 - Which Pico GPIOs are used for rows?
 - Which Pico GPIOs are used for columns?
 - Which expander pins (U2/U3) are used for rows?
 - Which expander pins (U2/U3) are used for columns?

3. **I2C Addresses:**
 - U2 I2C address? (typically 0x20-0x27)
 - U3 I2C address? (typically 0x21-0x27)

4. **Expander Pin Mapping:**
 - Which expander pins correspond to which matrix positions?
 - Port A vs Port B usage?

---

## Testing Checklist

- [ ] I2C communication works (can read/write expanders)
- [ ] Matrix scanning detects single key press
- [ ] Matrix scanning detects multiple key presses (polyphonic)
- [ ] Debouncing works (no false triggers)
- [ ] MIDI messages sent correctly
- [ ] HID keycodes sent correctly
- [ ] Fake input works (for testing without hardware)
- [ ] All 48 keys mapped correctly

---

## Next Steps

1. **Analyze schematic** to determine exact GPIO assignments
2. **Implement I2C driver** and test communication
3. **Implement matrix scanning** with known GPIO assignments
4. **Test with hardware** - single key, then multiple keys
5. **Add fake input** for development/debugging
6. **Tune debouncing** parameters
7. **Map all 48 keys** to MIDI notes and HID keycodes

Good luck! 

