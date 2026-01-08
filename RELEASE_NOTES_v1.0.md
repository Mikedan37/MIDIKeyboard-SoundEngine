# Release v1.0.0 – Open Source Release

## Initial Open Source Release

We're excited to announce the open-source release of the MIDI Keyboard project! This is a complete velocity-sensitive USB MIDI keyboard controller built on the Raspberry Pi Pico (RP2040). The device outputs USB MIDI events, and a Python-based host synthesizer provides real-time polyphonic audio synthesis.

## Key Features

### Hardware
- **25-Key Velocity-Sensitive Matrix**: Dual-contact detection per key for accurate velocity measurement (1-127 MIDI range)
- **Shift Register Interface**: Efficient column reading for matrix scanning
- **Raspberry Pi Pico (RP2040)**: Dual-core architecture for parallel processing
- **Open Design**: Schematic and PCB layout available (logically verified, not fabricated)

### Firmware
- **Real-Time Key Scanning**: Approximately 500 Hz scan rate with debouncing
- **Velocity Detection**: Precise timing-based velocity calculation from dual-contact timing
- **USB MIDI Class**: Standard USB MIDI device - no drivers required
- **Dual-Core Processing**: Core 1 handles GPIO polling and velocity timing, Core 0 manages USB communication

### Host Software
- **Polyphonic Synthesizer**: Pure Python implementation supporting all 25 keys simultaneously
- **macOS Integration**: Native menu bar application with live MIDI note display
- **Cross-Platform MIDI**: MIDI input works on macOS, Windows, and Linux
- **Audio Output**: Verified on macOS; Windows and Linux use default PortAudio device selection
- **Setup Scripts**: Automated installation and configuration scripts for all platforms

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

The setup scripts install Python dependencies, configure the synthesizer environment, and provide Pico detection support.

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

The project includes a test suite that validates firmware logic, velocity calculations, and matrix scanning behavior without requiring physical hardware:

```bash
cd qwerty_midi_pico/tests
make test_velocity_simple
./test_velocity_simple
```

## What's Included

- Complete firmware source code (C)
- Python synthesizer and GUI
- Hardware schematics and PCB layout design files
- Comprehensive test suite
- Full documentation
- Cross-platform setup scripts

## Technical Specifications

- **Microcontroller**: Raspberry Pi Pico (RP2040)
- **Keys**: 25 velocity-sensitive keys
- **Scan Rate**: Approximately 500–540 Hz
- **MIDI Velocity Range**: 1-127
- **Polyphony**: Full (all 25 keys simultaneously)
- **USB**: USB MIDI Class compliant
- **Platforms**: macOS (audio verified), Windows/Linux (MIDI input, default audio)

## Credits

- **Michael Danylchuk** - Firmware, Audio Engine, macOS GUI
- **Christopher "Zac" Hatchett** - Hardware Design, PCB Layout

## Acknowledgments

- San José State University (EE198A Senior Design Project)
- Dr. Nadir Mir - Project Mentor
- Raspberry Pi Foundation - Pico SDK
- TinyUSB Contributors - USB MIDI stack

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](docs/root_docs/CONTRIBUTING.md) for guidelines.

---

**Thank you for checking out our project! We hope this open-source release helps others learn and build upon our work.**
