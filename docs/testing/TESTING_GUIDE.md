# Test Testing Guide - No Hardware Required

## Quick Start

```bash
cd qwerty_midi_pico/tests
./run_tests.sh
```

That's it! All tests run without hardware.

---

## What Gets Tested

### Matrix Scanning Logic
- Row/column to key index conversion
- All 48 keys mapped correctly
- Matrix state management

### Fake Input System
- Simulated key presses work
- Key releases work
- Multiple keys simultaneously (polyphonic)

### MIDI Mapping
- All 48 keys → MIDI notes 60-107 (C4-B7)
- Chromatic progression verified
- Note ON/OFF messages

### HID Mapping
- All 48 keys → HID keycodes
- Valid keycode range verified
- Key press/release messages

### Integration Flow
- Fake input → Matrix scan → MIDI/HID output
- Polyphonic chords
- Rapid key presses
- All 48 keys individually

---

## Test Files

1. **`test_matrix.c`** - Matrix scanning and fake input
2. **`test_midi_hid.c`** - MIDI/HID mappings and message sending
3. **`test_integration.c`** - Complete end-to-end flow

---

## Running Tests

### Option 1: Run All (Recommended)
```bash
cd qwerty_midi_pico/tests
./run_tests.sh
```

### Option 2: Use Make
```bash
cd qwerty_midi_pico/tests
make run-all
```

### Option 3: Individual Tests
```bash
make test_matrix # Matrix scanning only
make test_midi_hid # MIDI/HID only
make test_integration # Integration only
```

---

## Expected Output

```

 KEYBOARD MATRIX TEST SUITE

[TEST] Testing matrix index conversion...
 Row 0, Col 0 → Key 0
 Row 0, Col 1 → Key 1
 ...
[TEST] Matrix index conversion PASSED

[TEST] Testing fake input system...
 Key 0 press detected
 Key 0 release detected
 ...
[TEST] Fake input system PASSED

...

 ALL TESTS PASSED! 

```

---

## How It Works

### Mock Functions
Tests use `mock_pico.c` to provide:
- GPIO functions (simulated, no hardware)
- Time functions (system clock)
- I2C functions (stubs)
- USB functions (stubs)

### Fake Input
- Simulates key presses without hardware
- Tests matrix scanning logic
- Verifies state management
- Tests polyphonic detection

### No Hardware Needed
- All tests run on your development machine
- Uses standard C libraries
- No Pico SDK required for tests
- No USB/MIDI hardware needed

---

## Troubleshooting

### Build Fails
```bash
# Check GCC is installed
gcc --version

# Clean and rebuild
make clean
make all
```

### Test Fails
- Check assertion messages
- Verify fake input is working
- Check matrix state

### Missing Functions
- Add to `mock_pico.c`
- Declare in `mock_pico.h`

---

## What This Proves

 **Matrix logic works** - Row/column scanning is correct 
 **Fake input works** - Can simulate key presses 
 **State management works** - Keys tracked correctly 
 **MIDI mapping works** - All 48 keys map correctly 
 **HID mapping works** - All 48 keys map correctly 
 **Integration works** - Full flow works end-to-end 

---

## After Tests Pass

1. Update GPIO pin assignments from schematic
2. Update I2C addresses if different
3. Compile firmware for Pico
4. Flash to hardware
5. Test with real keys

**Tests verify software logic before hardware testing!** 

