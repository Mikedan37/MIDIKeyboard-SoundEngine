# Velocity-Sensitive Keyboard Matrix - Implementation Complete

## Status: READY FOR HARDWARE

Complete firmware implementation for **2×24 velocity-sensitive keyboard matrix** with MSQT32 shift registers.

---

## Files Created

### Core Drivers
1. **`drivers/msqt32_shift_register.h/c`** - MSQT32 shift register interface
2. **`drivers/velocity_matrix.h/c`** - Velocity-sensitive matrix scanning
3. **`drivers/gpio_driver.h/c`** - Updated for velocity matrix

### Application
4. **`main.c`** - Updated for 24 keys with velocity

### Documentation
5. **`docs/VELOCITY_MATRIX_EXPLANATION.md`** - Complete explanation
6. **`docs/VELOCITY_IMPLEMENTATION_SUMMARY.md`** - This file

---

## Architecture

### Hardware → Firmware → MIDI

```
Physical Keys (24 keys)
 ↓
Early Contact (ROW0) + Late Contact (ROW1)
 ↓
MSQT32 Shift Registers (24 bits)
 ↓
velocity_matrix_scan() - 2-phase scanning
 ↓
Velocity Calculation (time delta)
 ↓
MIDI Note ON/OFF with Velocity (1-127)
```

---

## Key Features

### Velocity Detection
- **ROW0**: Early contact (first touch) → timestamp T0
- **ROW1**: Late contact (full press) → timestamp T1
- **Velocity**: Calculated from time delta (T1 - T0)
- **Range**: 1-127 (MIDI standard)

### MSQT32 Interface
- **24-bit shift register** read
- **DATA, CLOCK, LATCH** pins
- **3 banks** × 8 bits = 24 columns

### Debouncing
- **5ms debounce** time
- **50ms velocity timeout**
- **Per-key state** management

### MIDI Output
- **Note ON** with velocity (1-127)
- **Note OFF** messages
- **24 keys** mapped to MIDI notes 60-83 (C4-B5)

---

## Scanning Cycle

### Phase 1: Early Contact (ROW0)
```
1. Drive ROW0 = HIGH, ROW1 = LOW
2. Read 24 columns via MSQT32 → row0_columns
3. For each column:
 - If bit set → Record firstContactTime
```

### Phase 2: Late Contact (ROW1)
```
1. Drive ROW0 = LOW, ROW1 = HIGH
2. Read 24 columns via MSQT32 → row1_columns
3. For each column:
 - If bit set → Record secondContactTime
 - Calculate velocity from time delta
```

### Phase 3: State Update
```
1. Update key pressed state (with debouncing)
2. Calculate velocity if both contacts detected
3. Emit MIDI messages
```

---

## Configuration

### Pin Assignments (UPDATE FROM SCHEMATIC)

**File**: `drivers/velocity_matrix.h`
```c
#define ROW0_PIN 6 // Early contact row
#define ROW1_PIN 7 // Late contact row
```

**File**: `drivers/msqt32_shift_register.h`
```c
#define SHIFT_DATA_PIN 10 // Data input
#define SHIFT_CLOCK_PIN 11 // Clock output
#define SHIFT_LATCH_PIN 12 // Latch output
```

### Velocity Parameters
```c
#define MIN_VELOCITY_TIME_US 100 // Fastest = 127
#define MAX_VELOCITY_TIME_US 50000 // Slowest = 1
#define DEBOUNCE_TIME_US 5000 // 5ms debounce
```

---

## Key State Structure

```c
typedef struct {
 bool isPressed; // Current state
 bool wasPressed; // Previous state
 uint32_t firstContactTime; // ROW0 timestamp
 uint32_t secondContactTime; // ROW1 timestamp
 uint32_t releaseTime; // Release timestamp
 uint8_t velocity; // 1-127
 bool velocityComputed; // Flag
} key_state_t;
```

---

## Data Flow Diagram

```

 HARDWARE: Physical Keys 
 - 24 keys with dual contacts (early/late) 

 MSQT32 Shift Registers 
 - 24 bits (3 banks × 8 bits) 
 - DATA, CLOCK, LATCH interface 

 FIRMWARE: velocity_matrix_scan() 

 Phase 1: ROW0 scan → firstContactTime 
 Phase 2: ROW1 scan → secondContactTime 
 Phase 3: Calculate velocity from time delta 

 MIDI OUTPUT 
 - Note ON: 0x90, note, velocity (1-127) 
 - Note OFF: 0x80, note, 0 

```

---

## Velocity Calculation

### Formula
```c
if (time_delta < 100μs):
 velocity = 127 // Maximum
else if (time_delta > 50ms):
 velocity = 1 // Minimum
else:
 velocity = 127 - ((time_delta - 100μs) * 126 / 49900μs)
```

### Examples
- **100μs** → Velocity **127** (very fast)
- **1ms** → Velocity **~125** (fast)
- **10ms** → Velocity **~75** (medium)
- **50ms** → Velocity **1** (slow)

---

## Test Testing

### Compile
```bash
cd qwerty_midi_pico
mkdir -p build && cd build
cmake ..
make
```

### Flash to Pico
```bash
# Copy .uf2 file to Pico
cp qwerty_midi_pico.uf2 /Volumes/RPI-RP2/
```

### Verify
- Press keys and check MIDI output
- Verify velocity changes with press speed
- Check all 24 keys work

---

## Implementation Checklist

- [x] MSQT32 shift register driver
- [x] 2-phase matrix scanning (ROW0/ROW1)
- [x] Velocity calculation from time delta
- [x] Per-key state management
- [x] Debouncing (5ms)
- [x] MIDI output with velocity
- [x] HID keycode output
- [x] 24 keys mapped correctly
- [ ] **Update pin assignments from schematic** 
- [ ] **Test with hardware** 

---

## Required Updates

### Before Hardware Testing:

1. **Update Pin Assignments** in:
 - `drivers/velocity_matrix.h`: ROW0_PIN, ROW1_PIN
 - `drivers/msqt32_shift_register.h`: DATA, CLOCK, LATCH pins

2. **Verify MSQT32 Wiring**:
 - DATA pin connected correctly
 - CLOCK pin connected correctly
 - LATCH pin connected correctly
 - Power and ground connected

3. **Test Single Key First**:
 - Verify early contact detected
 - Verify late contact detected
 - Verify velocity calculated

---

## Summary

**Status**: **COMPLETE**

**Keys**: 24 (one per column) 
**Rows**: 2 (for velocity detection) 
**Interface**: MSQT32 shift registers 
**Velocity**: 1-127 (MIDI standard) 
**Output**: MIDI with velocity + HID 

**Ready for hardware testing after pin configuration!** 

