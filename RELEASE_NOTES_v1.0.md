# Release v1.0 - Open Source Release

## Initial Open Source Release

We're excited to announce the open-source release of the MIDI Keyboard project! This is a complete velocity-sensitive USB MIDI keyboard controller built on the Raspberry Pi Pico (RP2040) with real-time polyphonic synthesis.

## Key Features

### Hardware
- **2×25 Velocity-Sensitive Keyboard Matrix**: Dual-contact detection for accurate velocity measurement (1-127 MIDI range)
- **MSQT32 Shift Registers**: Efficient 24-bit column reading interface
- **Raspberry Pi Pico (RP2040)**: Dual-core architecture for parallel processing

### Firmware
- **Real-Time Key Scanning**: 500 Hz scan rate with 5ms debouncing
- **Velocity Detection**: Precise timing-based velocity calculation
- **USB MIDI Class**: Standard USB MIDI device - no drivers required
- **Dual-Core Processing**: Core 1 handles GPIO polling, Core 0 manages USB communication

### Software
- **Polyphonic Synthesizer**: Pure Python implementation supporting all 25 keys simultaneously
- **macOS Integration**: Native menu bar application with live MIDI note display
- **Cross-Platform MIDI**: MIDI input works on macOS, Windows, and Linux
- **Audio Output**: Verified on macOS; Windows and Linux use default PortAudio device selection
- **Auto-Launch**: Automatic setup scripts for all platforms

## Quick Start

### Automated Setup

**macOS/Linux:**
```bash
./setup.sh
```

**Windows:**
```powershell
.\setup.ps1
```

The setup script automatically installs dependencies, configures auto-launch, and detects your Pico connection.

### Building Firmware

```bash
cd qwerty_midi_pico
mkdir build && cd build
cmake ..
make
```

Flash the generated `.uf2` file to your Pico. See [FLASH.md](qwerty_midi_pico/FLASH.md) for detailed instructions.

## Documentation

Comprehensive documentation is available in the `docs/` directory:
- [Installation Guide](INSTALL.md)
- [Hardware Documentation](docs/hardware/)
- [System Architecture](docs/architecture/SYSTEM_DESIGN.md)
- [Testing Guide](docs/testing/TESTING_GUIDE.md)

## Testing

The project includes a comprehensive test suite that validates functionality without requiring physical hardware:

```bash
cd qwerty_midi_pico/tests
make test_velocity_simple
./test_velocity_simple
```

## What's Included

- Complete firmware source code (C)
- Python synthesizer and GUI
- Hardware schematics and PCB design files
- Comprehensive test suite
- Full documentation
- Cross-platform setup scripts
- Example configurations

## Technical Specifications

- **Microcontroller**: Raspberry Pi Pico (RP2040)
- **Keys**: 25 velocity-sensitive keys
- **Scan Rate**: 500 Hz
- **MIDI Velocity Range**: 1-127
- **Polyphony**: Full (all 25 keys simultaneously)
- **USB**: USB MIDI Class compliant
- **Platforms**: macOS, Windows, Linux

## Credits

- **Michael Danylchuk** - Firmware, Audio Engine, macOS GUI
- **Christopher "Zac" Hatchett** - Hardware Design, PCB, GPIO Integration

## Acknowledgments

- San Jose State University (EE198A Senior Design Project)
- Dr. Nadir Mir - Project Mentor
- Raspberry Pi Foundation - Pico SDK
- TinyUSB Contributors - USB MIDI stack

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](docs/root_docs/CONTRIBUTING.md) for guidelines.

---

**Thank you for checking out our project! We hope this open-source release helps others learn and build upon our work.**

