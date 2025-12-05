# Hardware Upgrade Analysis: Direct GPIO → Keyboard Matrix + I2C

## Current vs. New Hardware

### Current Implementation (Simple)
- **5 keys** directly connected to GPIO pins 0-4
- **Simple polling**: Just read GPIO state directly
- **No I2C**: All keys on Pico GPIOs
- **No matrix**: One GPIO per key

### New Hardware (From Schematic)
- **48 keys** (SW1-SW48) arranged in keyboard matrix
- **I2C GPIO Expanders**: U2 and U3 (likely MCP23017 - 16 GPIO each)
- **Matrix Configuration**: Row/column scanning with diodes (D1-D48)
- **Pico GPIOs**: Used for matrix rows/columns + I2C communication (GPIO4, GPIO5)

---

## Impact Assessment: How Hard Is This Change?

### Difficulty Level: **MODERATE** 

**Why it's not trivial:**
1. Need I2C driver implementation
2. Need matrix scanning algorithm
3. Need proper debouncing for matrix
4. Need key mapping (matrix position → key number)
5. Need simulation/fake input support

**Why it's manageable:**
1. Pico SDK has I2C support built-in
2. Matrix scanning is well-documented
3. Current architecture is modular (easy to swap driver)
4. Most of the USB/MIDI code stays the same

---

## Required Changes Breakdown

### 1. **EASY**: Update Key Count
**Files**: `gpio_driver.h`, `main.c`
```c
// Change from:
#define NUM_KEYS 5

// To:
#define NUM_KEYS 48
```

**Impact**: Low - just constant changes

---

### 2. **MODERATE**: Add I2C Driver
**New File**: `drivers/i2c_gpio_expander.c/h`

**What's needed:**
- I2C initialization (Pico SDK has this)
- MCP23017 register definitions
- Functions to read/write GPIO expander pins
- Handle both U2 and U3 (different I2C addresses)

**Pico SDK I2C Example:**
```c
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define I2C_SDA 4 // GPIO4
#define I2C_SCL 5 // GPIO5
#define I2C_FREQ 400000 // 400kHz

void i2c_init() {
 i2c_init(I2C_PORT, I2C_FREQ);
 gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
 gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
 gpio_pull_up(I2C_SDA);
 gpio_pull_up(I2C_SCL);
}
```

**MCP23017 Addresses** (typical):
- U2: 0x20 (A0=0, A1=0, A2=0)
- U3: 0x21 (A0=1, A1=0, A2=0) or 0x22

**Estimated LOC**: ~150-200 lines

---

### 3. **MODERATE**: Implement Matrix Scanning
**New File**: `drivers/keyboard_matrix.c/h`

**Matrix Scanning Algorithm:**
```c
// Pseudocode
void scan_matrix() {
 for (each row) {
 // Set row HIGH (output)
 set_row(row, HIGH);

 // Read all columns (input)
 for (each column) {
 bool pressed = read_column(column);
 int key_index = matrix_to_key_index(row, column);
 update_key_state(key_index, pressed);
 }

 // Set row LOW (output)
 set_row(row, LOW);
 }
}
```

**Key Considerations:**
- **Diodes prevent ghosting** (already in hardware - good!)
- **Debouncing**: Need software debouncing (50ms typical)
- **Scan rate**: ~1-5ms per row (48 keys = ~10-50ms total scan)
- **Row/Column GPIO assignment**: Need to determine from schematic

**Estimated LOC**: ~200-250 lines

---

### 4. **EASY**: Update Key Mapping
**File**: `main.c`

**Current:**
```c
const uint8_t midi_notes[NUM_KEYS] = {60, 62, 64, 65, 67};
const uint8_t hid_keycodes[NUM_KEYS] = {0x04, 0x05, 0x06, 0x07, 0x08};
```

**New:**
```c
// 48 keys - need to map each matrix position to MIDI note and HID keycode
const uint8_t midi_notes[NUM_KEYS] = {
 // Full mapping for 48 keys
 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107
};

const uint8_t hid_keycodes[NUM_KEYS] = {
 // Standard QWERTY layout mapping
 // ... (48 keycodes)
};
```

**Impact**: Medium - need to define full key layout

---

### 5. **MODERATE**: Refactor GPIO Driver
**File**: `drivers/gpio_driver.c/h`

**Current Approach:**
```c
// Direct GPIO read
bool gpio_is_pressed(uint8_t pin) {
 return gpio_get(pin);
}
```

**New Approach:**
```c
// Matrix-based key read
bool gpio_is_pressed(uint8_t key_index) {
 return key_matrix_state[key_index];
}
```

**Changes Needed:**
- Replace direct GPIO reads with matrix scan results
- Update `gpio_poll_loop()` to call matrix scanner
- Keep same interface for `main.c` (abstraction layer)

**Impact**: Medium - need to maintain API compatibility

---

### 6. **MODERATE**: Add Debouncing
**New File**: `drivers/debounce.c/h` (or add to matrix driver)

**Why needed:**
- Matrix scanning can be noisy
- Mechanical switches bounce
- Need time-based debouncing

**Implementation:**
```c
typedef struct {
 bool current_state;
 bool stable_state;
 uint32_t last_change_time;
 uint32_t debounce_time_ms;
} debounce_state_t;

bool debounce_key(debounce_state_t *state, bool raw_input, uint32_t now) {
 if (raw_input != state->current_state) {
 state->current_state = raw_input;
 state->last_change_time = now;
 }

 if ((now - state->last_change_time) > state->debounce_time_ms) {
 state->stable_state = state->current_state;
 }

 return state->stable_state;
}
```

**Estimated LOC**: ~50-100 lines

---

### 7. **EASY**: Add Simulation/Fake Input
**New File**: `drivers/fake_input.c/h` or update `sim/`

**Why needed:**
- Can't test without hardware
- Development/debugging
- Demo purposes

**Implementation Options:**

**Option A: Random Key Presses**
```c
void fake_matrix_input() {
 static uint32_t last_fake = 0;
 uint32_t now = time_us_32();

 if ((now - last_fake) > 1000000) { // Every 1 second
 int random_key = rand() % NUM_KEYS;
 simulate_key_press(random_key);
 last_fake = now;
 }
}
```

**Option B: Sequence/Pattern**
```c
void fake_matrix_input() {
 static int sequence[] = {0, 4, 7, 12, 16, 19, 24, 28, 31, 36, 40, 43};
 static int seq_index = 0;

 // Cycle through sequence
 simulate_key_press(sequence[seq_index]);
 seq_index = (seq_index + 1) % (sizeof(sequence)/sizeof(sequence[0]));
}
```

**Option C: Serial Command Interface**
```c
// Receive commands via serial: "PRESS:12" or "RELEASE:12"
void handle_serial_fake_input() {
 char cmd[32];
 if (serial_read(cmd, sizeof(cmd))) {
 if (strncmp(cmd, "PRESS:", 6) == 0) {
 int key = atoi(cmd + 6);
 simulate_key_press(key);
 }
 }
}
```

**Recommended**: Option C (most flexible for testing)

**Estimated LOC**: ~100-150 lines

---

## File Structure Changes

### New Files Needed:
```
qwerty_midi_pico/
 drivers/
 gpio_driver.c/h [MODIFY - replace direct GPIO with matrix]
 i2c_gpio_expander.c/h [NEW - I2C communication]
 keyboard_matrix.c/h [NEW - matrix scanning]
 debounce.c/h [NEW - debouncing logic]
 sim/
 fake_input.c/h [NEW - simulation support]
 sim_matrix.c [NEW - matrix simulation]
 main.c [MODIFY - update key mappings]
```

### Files to Modify:
1. `drivers/gpio_driver.c/h` - Replace direct GPIO with matrix interface
2. `main.c` - Update NUM_KEYS and key mappings
3. `CMakeLists.txt` - Add new source files

---

## Implementation Plan

### Phase 1: I2C Driver (Foundation)
1. Add I2C initialization
2. Implement MCP23017 register access
3. Test I2C communication (read/write GPIO expander)

**Time Estimate**: 2-3 hours

### Phase 2: Matrix Scanning
1. Determine row/column GPIO assignments from schematic
2. Implement matrix scan algorithm
3. Test with single key press
4. Test with multiple key presses (polyphonic)

**Time Estimate**: 3-4 hours

### Phase 3: Integration
1. Replace GPIO driver with matrix driver
2. Update key mappings (48 keys)
3. Add debouncing
4. Test full system

**Time Estimate**: 2-3 hours

### Phase 4: Simulation/Fake Input
1. Implement fake input system
2. Add serial command interface
3. Test without hardware

**Time Estimate**: 1-2 hours

**Total Estimated Time**: 8-12 hours

---

## Key Questions to Answer

### 1. Matrix Configuration
- **How many rows?** (likely 6-8 rows)
- **How many columns?** (likely 6-8 columns)
- **Which GPIOs are rows?** (Pico GPIOs or expander?)
- **Which GPIOs are columns?** (Pico GPIOs or expander?)

### 2. I2C Expander Details
- **Exact chip model?** (MCP23017? MCP23S17? Other?)
- **I2C addresses?** (U2 and U3)
- **Which pins on expanders are used?** (all 16? subset?)

### 3. Key Layout
- **Physical key arrangement?** (QWERTY? Piano? Custom?)
- **MIDI note mapping?** (chromatic? scale? custom?)
- **HID keycode mapping?** (standard keyboard? custom?)

---

## Code Example: New Architecture

### New `gpio_driver.h` Interface:
```c
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "pico/stdlib.h"

#define NUM_KEYS 48

// Matrix configuration (need to determine from schematic)
#define MATRIX_ROWS 8
#define MATRIX_COLS 6

void gpio_driver_init();
void gpio_poll_loop(); // Scans matrix on Core 1
bool gpio_is_pressed(uint8_t key_index); // Query key state

// Matrix-specific functions
void matrix_scan(); // Scan entire matrix
bool matrix_get_key(uint8_t row, uint8_t col); // Get specific matrix position

#endif
```

### New `keyboard_matrix.c` Structure:
```c
#include "keyboard_matrix.h"
#include "i2c_gpio_expander.h"

// Matrix state
static bool key_matrix_state[MATRIX_ROWS][MATRIX_COLS] = {false};
static bool key_stable_state[NUM_KEYS] = {false};

// Row/Column GPIO assignments (need from schematic)
const uint row_pins[MATRIX_ROWS] = {GPIO_X, GPIO_Y, ...};
const uint col_pins[MATRIX_COLS] = {GPIO_A, GPIO_B, ...};

void matrix_scan() {
 for (int row = 0; row < MATRIX_ROWS; row++) {
 // Set row HIGH
 set_row_output(row, true);
 sleep_us(10); // Small delay for settling

 // Read all columns
 for (int col = 0; col < MATRIX_COLS; col++) {
 bool pressed = read_column_input(col);
 key_matrix_state[row][col] = pressed;

 // Update debounced state
 int key_index = row * MATRIX_COLS + col;
 key_stable_state[key_index] = debounce_key(
 &debounce_states[key_index],
 pressed,
 time_us_32()
 );
 }

 // Set row LOW
 set_row_output(row, false);
 }
}

int matrix_to_key_index(uint8_t row, uint8_t col) {
 return row * MATRIX_COLS + col;
}
```

---

## Testing Strategy

### 1. Unit Tests (Without Hardware)
- Fake I2C responses
- Test matrix scanning logic
- Test debouncing algorithm
- Test key mapping

### 2. Integration Tests (With Hardware)
- Single key press
- Multiple key presses (polyphonic)
- Rapid key presses
- Key release detection

### 3. Simulation Tests
- Fake input generates correct MIDI
- Fake input generates correct HID
- Serial command interface works

---

## Risk Assessment

### Low Risk 
- I2C driver (Pico SDK support)
- USB/MIDI code (no changes needed)
- Python audio engine (no changes needed)

### Medium Risk 
- Matrix scanning timing (need to tune)
- Debouncing parameters (need to tune)
- I2C communication reliability

### High Risk 
- None identified - architecture is sound

---

## Recommendations

### 1. **Start with I2C Driver**
 - Get basic I2C communication working first
 - Test with simple read/write operations
 - Verify expander addresses

### 2. **Implement Matrix Scanning Incrementally**
 - Start with 1 row, 1 column
 - Expand to full matrix
 - Test with single key, then multiple keys

### 3. **Add Simulation Early**
 - Implement fake input before hardware is ready
 - Allows development/testing without hardware
 - Use serial commands for flexibility

### 4. **Maintain Backward Compatibility**
 - Keep same API in `gpio_driver.h`
 - `main.c` shouldn't need major changes
 - Abstract matrix complexity in driver layer

### 5. **Document Matrix Layout**
 - Create mapping table: (row, col) → key_index → MIDI note
 - Document which physical keys map to which positions
 - This will be critical for debugging

---

## Conclusion

**Is this a hard change?** 

**Answer: MODERATE difficulty** - Not trivial, but very doable.

**Key Factors:**
- Good foundation (modular code)
- Pico SDK has I2C support
- Matrix scanning is well-understood
- Need to determine exact matrix configuration
- Need to implement I2C driver
- Need proper debouncing

**Estimated Effort**: 8-12 hours of focused development

**Recommendation**: 
1. Start with I2C driver (foundation)
2. Add matrix scanning (core functionality)
3. Add simulation/fake input (enables testing)
4. Integrate and test (polish)

The architecture is sound - this is primarily adding new driver layers while keeping the USB/MIDI code unchanged.

