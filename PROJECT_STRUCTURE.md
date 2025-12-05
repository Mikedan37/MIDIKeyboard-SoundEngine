# Project Structure

This document outlines the organization of the QWERTY MIDI Keyboard project.

## Directory Layout

```
.
 LICENSE # MIT License
 README.md # Main project documentation
 CONTRIBUTING.md # Contribution guidelines
 CHANGELOG.md # Version history
 .gitignore # Git ignore rules

 qwerty_midi_pico/ # Raspberry Pi Pico firmware
 CMakeLists.txt # Build configuration
 main.c # Firmware entry point
 usb_descriptors.c/h # USB HID + MIDI descriptors
 tusb_config.h # TinyUSB configuration
 config.h # Project configuration

 drivers/ # Hardware drivers
 velocity_matrix.c/h # Velocity-sensitive matrix
 msqt32_shift_register.c/h # Shift register interface
 gpio_driver.c/h # GPIO abstraction
 keyboard_matrix.c/h # Matrix scanning (legacy)
 i2c_gpio_expander.c/h # I2C expander (legacy)
 fake_input.c/h # Test input simulation

 tests/ # Test suite
 README.md # Test documentation
 Makefile # Test build system
 run_tests.sh # Test runner script
 test_velocity_simple.c # Velocity calculation tests
 test_velocity_matrix.c # Full matrix tests
 mock_pico.c/h # Pico SDK mocks
 ...

 sim/ # Simulation code (legacy)
 ...

 build/ # Build artifacts (gitignored)

 midi_sound_engine/ # Python synthesizer
 requirements.txt # Python dependencies
 engine.py # Core synthesis engine
 synth_menu.py # macOS menu bar GUI
 pico_listener.py # MIDI input handler
 monitor_and_launch.py # Auto-launcher
 unified_listener.py # Unified input handler
 ...

 docs/ # Documentation
 README.md # Documentation index
 SYSTEM_DESIGN.md # Architecture overview
 VELOCITY_MATRIX_EXPLANATION.md
 TESTING_GUIDE.md
 ...
```

## Key Directories

### `qwerty_midi_pico/`
Firmware for the Raspberry Pi Pico microcontroller.

**Key Files:**
- `main.c` - Main firmware loop
- `drivers/velocity_matrix.c` - Velocity detection logic
- `drivers/msqt32_shift_register.c` - Shift register interface
- `usb_descriptors.c` - USB device descriptors

### `midi_sound_engine/`
Python-based real-time synthesizer and GUI.

**Key Files:**
- `engine.py` - Audio synthesis engine
- `synth_menu.py` - macOS menu bar interface
- `pico_listener.py` - MIDI input handler

### `docs/`
Comprehensive project documentation.

**Categories:**
- Architecture & Design
- Hardware Documentation
- Implementation Guides
- Testing Documentation

## Important Files

### Root Level
- **README.md** - Main project documentation
- **LICENSE** - MIT License
- **CONTRIBUTING.md** - How to contribute
- **CHANGELOG.md** - Version history
- **.gitignore** - Git ignore rules

### Configuration
- `qwerty_midi_pico/CMakeLists.txt` - Build system
- `midi_sound_engine/requirements.txt` - Python dependencies

## 🧹 Cleanup

The following are excluded from version control (via .gitignore):
- Build artifacts (`build/`, `*.o`, `*.elf`, `*.uf2`)
- Python cache (`__pycache__/`, `*.pyc`)
- Virtual environments (`venv/`, `env/`)
- IDE files (`.vscode/`, `.idea/`)
- Test artifacts

## Distribution

When packaging for release:
1. Exclude build artifacts
2. Include all source files
3. Include documentation
4. Include LICENSE and README
5. Include requirements.txt

## Finding Files

- **Firmware code**: `qwerty_midi_pico/drivers/`
- **Tests**: `qwerty_midi_pico/tests/`
- **Synthesizer**: `midi_sound_engine/`
- **Documentation**: `docs/`
- **Configuration**: Root level files

