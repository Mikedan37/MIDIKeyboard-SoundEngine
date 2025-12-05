# Velocity-Sensitive Keyboard Matrix - Complete Explanation

## Hardware Architecture

### Matrix Configuration
- **2 Rows**: Used for velocity detection (not separate keys)
 - **ROW0**: Early contact (first touch)
 - **ROW1**: Late contact (full press)
- **24 Columns**: One key per column = **24 keys total**
- **MSQT32 Shift Registers**: Read 24 column bits via shift register interface

### Key Detection Flow

```
Physical Key Press
 ↓
Early Contact (ROW0) → Timestamp T0
 ↓
Late Contact (ROW1) → Timestamp T1
 ↓
Velocity = f(T1 - T0) // Faster press = higher velocity
```

---

## Scanning Cycle

### Phase 1: Early Contact Detection
```
1. Drive ROW0 = HIGH, ROW1 = LOW
2. Read 24 columns via MSQT32 → row0_columns (24-bit mask)
3. For each column:
 - If bit set → Record firstContactTime = now
```

### Phase 2: Late Contact Detection
```
1. Drive ROW0 = LOW, ROW1 = HIGH
2. Read 24 columns via MSQT32 → row1_columns (24-bit mask)
3. For each column:
 - If bit set → Record secondContactTime = now
 - Calculate velocity: time_delta = secondContactTime - firstContactTime
```

### Phase 3: Velocity Calculation
```
For each key:
 if (firstContactTime > 0 && secondContactTime > 0):
 time_delta = secondContactTime - firstContactTime
 velocity = calculate_velocity(time_delta)
 // Faster press (shorter delta) = higher velocity
```

---

## Data Flow

```

 HARDWARE LAYER 

 Physical Keys (24 keys) 
 ↓ 
 Early Contact (ROW0) → GPIO ROW0_PIN 
 Late Contact (ROW1) → GPIO ROW1_PIN 
 Column States → MSQT32 Shift Registers 
 ↓ 
 MSQT32 Interface: 
 - DATA pin (read column bits) 
 - CLOCK pin (shift clock) 
 - LATCH pin (latch state) 

 ↓

 FIRMWARE LAYER 

 velocity_matrix_scan() 
 Phase 1: Read ROW0 (early contact) 
 Phase 2: Read ROW1 (late contact) 
 Phase 3: Calculate velocity per key 

 Per-Key State: 
 - firstContactTime (ROW0 timestamp) 
 - secondContactTime (ROW1 timestamp) 
 - velocity (1-127, calculated from time delta) 
 - isPressed (debounced state) 

 ↓

 MIDI OUTPUT 

 send_midi_note(note, true, velocity) 
 - Note ON message 
 - Velocity included (1-127) 

 send_midi_note(note, false, 0) 
 - Note OFF message 

```

---

## Velocity Calculation

### Formula
```c
velocity = 127 - ((time_delta - MIN_TIME) * 126 / TIME_RANGE)
```

### Parameters
- **MIN_VELOCITY_TIME_US**: 100μs (fastest press = velocity 127)
- **MAX_VELOCITY_TIME_US**: 50,000μs (slowest press = velocity 1)
- **Velocity Range**: 1-127 (MIDI standard)

### Examples
- **100μs delta** → Velocity 127 (very fast press)
- **1,000μs delta** → Velocity ~125 (fast press)
- **10,000μs delta** → Velocity ~75 (medium press)
- **50,000μs delta** → Velocity 1 (slow press)

---

## Key State Structure

```c
typedef struct {
 bool isPressed; // Current debounced state
 bool wasPressed; // Previous frame state
 uint32_t firstContactTime; // ROW0 timestamp (early)
 uint32_t secondContactTime; // ROW1 timestamp (late)
 uint32_t releaseTime; // Release timestamp
 uint8_t velocity; // Computed velocity (1-127)
 bool velocityComputed; // Velocity calculated flag
} key_state_t;
```

---

## MSQT32 Shift Register Interface

### Pin Functions
- **DATA**: Read column state bits (input)
- **CLOCK**: Shift clock (output)
- **LATCH**: Latch current state (output)

### Read Sequence
```
1. Pulse LATCH (capture current state)
2. For each of 24 bits:
 - Read DATA pin
 - Pulse CLOCK (shift next bit)
3. Return 24-bit mask
```

### Bit Mapping
- **Bits 0-7**: Bank 0 (P00-P07)
- **Bits 8-15**: Bank 1 (P10-P17)
- **Bits 16-23**: Bank 2 (P20-P27)

---

## Debouncing

### Configuration
- **Debounce Time**: 5ms (DEBOUNCE_TIME_US = 5000)
- **Velocity Timeout**: 50ms (max time between contacts)

### Logic
```
Key Press:
 - Record timestamps when contacts detected
 - Only mark as pressed after debounce time
 - Calculate velocity when both contacts detected

Key Release:
 - Reset all timestamps
 - Mark as released after debounce time
```

---

## MIDI Message Format

### Note ON
```
Byte 0: 0x90 (Note ON, Channel 0)
Byte 1: Note number (0-127)
Byte 2: Velocity (1-127)
```

### Note OFF
```
Byte 0: 0x80 (Note OFF, Channel 0)
Byte 1: Note number (0-127)
Byte 2: 0 (velocity = 0 for note off)
```

---

## Pin Configuration

### Update These Constants

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

---

## Scanning Timing

### Scan Rate
- **Scan Interval**: 2ms (~500Hz)
- **Row Settling Time**: 10μs
- **Latch Pulse**: 1μs
- **Clock Pulse**: 1μs

### Total Scan Time
- Phase 1 (ROW0): ~1ms
- Phase 2 (ROW1): ~1ms
- **Total**: ~2ms per scan cycle

---

## Velocity Detection Edge Cases

### Case 1: Only ROW0 Detected
- Very fast press, ROW1 contact missed
- **Solution**: Use maximum velocity (127)

### Case 2: Only ROW1 Detected
- ROW0 contact missed
- **Solution**: Use maximum velocity (127)

### Case 3: Timeout (>50ms between contacts)
- Slow or inconsistent press
- **Solution**: Use default velocity (64)

### Case 4: Both Contacts Simultaneous
- Perfect timing
- **Solution**: Maximum velocity (127)

---

## Performance Characteristics

### Latency
- **Scan Latency**: ~2ms
- **Debounce Latency**: 5ms
- **Total Key Detection**: ~7ms
- **MIDI Output**: <1ms

### Throughput
- **Scan Rate**: 500Hz
- **Max Keys**: 24 simultaneous
- **Velocity Resolution**: 1-127 (127 levels)

---

## Testing Without Hardware

The firmware includes:
- MSQT32 shift register interface (ready for hardware)
- Velocity calculation logic (tested)
- Debouncing (tested)
- MIDI output with velocity (ready)

**To test with hardware:**
1. Update pin assignments
2. Connect MSQT32 shift registers
3. Connect row pins
4. Flash firmware
5. Press keys and verify velocity

---

## Summary

**Architecture**: 2×24 matrix with velocity detection 
**Keys**: 24 total (one per column) 
**Rows**: Used for velocity (early/late contact) 
**Interface**: MSQT32 shift registers (24 bits) 
**Velocity**: 1-127 based on contact timing 
**Output**: MIDI with velocity, HID keycodes 

**Complete and ready for hardware testing!** 

