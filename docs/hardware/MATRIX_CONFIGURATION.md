# Keyboard Matrix Configuration Guide

## Hardware Reference

### Circuit Schematic

![Circuit Schematic](../../hardware/hardware_pics/schematic.png)

Refer to the schematic to identify:
- GPIO pin assignments for rows and columns
- MSQT32 shift register connections (U2, U3)
- I2C addresses (if applicable)
- Power and ground connections

### PCB Layout

![PCB Layout](../../hardware/hardware_pics/pcb.png)

The PCB layout shows the physical arrangement:
- Switch positions (SW1-SW50)
- Trace routing between components
- Component placement and orientation

## CRITICAL: Update These Values from Your Schematic

The implementation is complete, but you **MUST** update the GPIO pin assignments based on your actual hardware schematic.

---

## Files to Update

### 1. `drivers/keyboard_matrix.c`

Update these arrays based on your schematic:

```c
// Which GPIO pins are used for rows?
const uint matrix_row_pins[MATRIX_ROWS] = {
 6, 7, 8, 9, 10, 11, 12, 13 // UPDATE THESE!
};

// Which GPIO pins are used for columns?
const uint matrix_col_pins[MATRIX_COLS] = {
 14, 15, 16, 17, 18, 19 // UPDATE THESE!
};

// Which expander handles each row? (0=Pico, 1=U2, 2=U3)
const uint8_t row_expander[MATRIX_ROWS] = {
 0, 0, 0, 0, 1, 1, 1, 1 // UPDATE THESE!
};

// Which expander handles each column? (0=Pico, 1=U2, 2=U3)
const uint8_t col_expander[MATRIX_COLS] = {
 0, 0, 0, 1, 1, 1 // UPDATE THESE!
};

// Which expander pin (0-15) corresponds to each row?
const uint8_t row_expander_pin[MATRIX_ROWS] = {
 0, 0, 0, 0, 0, 1, 2, 3 // UPDATE THESE!
};

// Which expander pin (0-15) corresponds to each column?
const uint8_t col_expander_pin[MATRIX_COLS] = {
 0, 0, 0, 4, 5, 6 // UPDATE THESE!
};
```

### 2. `drivers/i2c_gpio_expander.h`

Update I2C addresses if different:

```c
#define EXPANDER_U2_ADDR 0x20 // UPDATE if different
#define EXPANDER_U3_ADDR 0x21 // UPDATE if different
```

---

## How to Determine Values from Schematic

### Step 1: Identify Matrix Size
- Count total keys: Should be 48
- Determine rows × columns (e.g., 8×6, 6×8, 12×4)
- Update `MATRIX_ROWS` and `MATRIX_COLS` in `keyboard_matrix.h` if needed

### Step 2: Identify Row GPIOs
- Look for switches connected to row lines
- Find which Pico GPIOs drive rows (outputs)
- Find which expander pins drive rows (if any)

### Step 3: Identify Column GPIOs
- Look for switches connected to column lines
- Find which Pico GPIOs read columns (inputs)
- Find which expander pins read columns (if any)

### Step 4: Identify I2C Addresses
- Check A0, A1, A2 pins on U2 and U3
- Calculate addresses:
 - Address = 0x20 + (A2<<2) + (A1<<1) + A0
- Example: A0=0, A1=0, A2=0 → 0x20
- Example: A0=1, A1=0, A2=0 → 0x21

### Step 5: Map Expander Pins
- MCP23017 has 16 GPIOs: Port A (0-7) and Port B (8-15)
- Determine which expander pin connects to which matrix row/col
- Update `row_expander_pin[]` and `col_expander_pin[]` arrays

---

## Testing Without Hardware

Fake input is **enabled by default** for testing:

1. **Enable/Disable**: Edit `drivers/fake_input.h`
 ```c
 #define ENABLE_FAKE_INPUT 1 // 1 = enabled, 0 = disabled
 ```

2. **Test Sequence**: Edit `drivers/fake_input.c` to change test pattern

3. **Manual Control**: Call `fake_input_simulate_key(key_index, pressed)` from code

---

## Matrix Scanning Algorithm

The matrix is scanned row-by-row:

1. Set one row HIGH (output)
2. Read all columns (inputs)
3. Set row LOW
4. Repeat for all rows
5. Debounce results (5ms)

**Scan Rate**: ~2ms per scan = 500Hz (fast enough for real-time)

---

## Troubleshooting

### No Keys Detected
- Check I2C addresses match hardware
- Check GPIO pin assignments
- Check expander pin mappings
- Enable fake input to test software

### Ghost Keys (Multiple keys pressed when only one is)
- Diodes should prevent this (hardware)
- Check matrix scanning logic
- Verify row/column assignments

### Keys Not Debouncing
- Adjust `debounce_time_us` in `keyboard_matrix.c` (default: 5000us = 5ms)

### I2C Communication Fails
- Check I2C pull-up resistors
- Verify I2C addresses
- Check wiring (SDA/SCL)
- Verify I2C frequency (400kHz default)

---

## Current Default Configuration

**Matrix**: 8 rows × 6 columns = 48 keys

**Rows**:
- Rows 0-3: Pico GPIO 6, 7, 8, 9
- Rows 4-7: U2 expander pins 0, 1, 2, 3

**Columns**:
- Cols 0-2: Pico GPIO 14, 15, 16
- Cols 3-5: U2 expander pins 4, 5, 6

**I2C**:
- U2: 0x20
- U3: 0x21

** These are PLACEHOLDERS - Update from your schematic!**

---

## Next Steps

1. Analyze schematic to get exact GPIO assignments
2. Update `keyboard_matrix.c` with correct values
3. Update I2C addresses if different
4. Test with fake input first (software test)
5. Test with hardware (single key, then multiple)
6. Tune debouncing if needed
7. Verify all 48 keys work

Good luck! 

