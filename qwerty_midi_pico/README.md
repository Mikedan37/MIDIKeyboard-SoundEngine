# QWERTY MIDI Pico Firmware

Firmware for the Raspberry Pi Pico (RP2040) implementing a velocity-sensitive USB keyboard with dual HID/MIDI modes.

## Structure

```
qwerty_midi_pico/
 main.c # Firmware entry point
 CMakeLists.txt # Build configuration
 config.h # Project configuration
 tusb_config.h # TinyUSB configuration
 usb_descriptors.c/h # USB HID + MIDI descriptors
 pico_sdk_import.cmake # Pico SDK import
 flash.sh # Flash script

 drivers/ # Hardware drivers
 current/ # Active drivers
 velocity_matrix.c/h # Velocity-sensitive matrix
 msqt32_shift_register.c/h # Shift register interface
 gpio_driver.c/h # GPIO abstraction
 legacy/ # Legacy drivers (reference only)

 tests/ # Test suite
 test_velocity_simple.c # Velocity calculation tests
 mock_pico.c/h # Pico SDK mocks

 legacy_simulation/ # Legacy simulation code
 legacy_pico/ # Legacy Pico code
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

Flash the generated `.uf2` file to your Pico by holding BOOTSEL and connecting via USB.

## Testing

```bash
cd tests
make test_velocity_simple
./test_velocity_simple
```

See [tests/README.md](tests/README.md) for complete testing instructions.

## Documentation

- Main docs: [../docs/](../docs/)
- Velocity matrix: [../docs/hardware/VELOCITY_MATRIX_EXPLANATION.md](../docs/hardware/VELOCITY_MATRIX_EXPLANATION.md)
- System design: [../docs/architecture/SYSTEM_DESIGN.md](../docs/architecture/SYSTEM_DESIGN.md)
- Drivers: [drivers/README.md](drivers/README.md)
- Flashing: [FLASH.md](FLASH.md)

## Configuration

### Pin Assignments

Update in `drivers/current/velocity_matrix.h`:
```c
#define ROW0_PIN 6 // Early contact row
#define ROW1_PIN 7 // Late contact row
```

Update in `drivers/current/msqt32_shift_register.h`:
```c
#define SHIFT_DATA_PIN 10 // MSQT32 data
#define SHIFT_CLOCK_PIN 11 // MSQT32 clock
#define SHIFT_LATCH_PIN 12 // MSQT32 latch
```

## Key Features

- Velocity-sensitive key detection (1-127 MIDI range)
- 2-phase scanning (early/late contact)
- USB HID keyboard mode
- USB MIDI controller mode
- 24 keys with velocity detection
- Comprehensive test suite
