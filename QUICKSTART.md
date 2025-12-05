# Quick Start Guide

Get up and running in 5 minutes.

## Fastest Setup

### 1. Run Setup Script

**macOS / Linux:**
```bash
./setup.sh
```

**Windows:**
```powershell
.\setup.ps1
```

### 2. Flash Firmware

1. Hold **BOOTSEL** on Pico
2. Connect via USB
3. Copy `qwerty_midi_pico/build/qwerty_midi_pico.uf2` to `RPI-RP2` drive
4. Done! Pico reboots automatically

### 3. Plug In and Play

1. Plug in your Pico
2. Synthesizer auto-launches
3. Start playing!

## What You Need

- **Raspberry Pi Pico** (hardware)
- **Python 3.9+** (auto-installed by setup script)
- **USB cable** (data cable, not charge-only)

## That's It!

The setup script handles everything:
- Python environment
- Dependencies
- Auto-launch configuration
- Monitoring setup

## Need More Help?

- **Detailed setup**: [INSTALL.md](INSTALL.md)
- **Firmware flashing**: [qwerty_midi_pico/FLASH.md](qwerty_midi_pico/FLASH.md)
- **Troubleshooting**: [docs/](docs/)

## Ready to Play!

Once setup is complete:
- Plug in Pico → Auto-detected
- Press keys → Hear music
- Menu bar (macOS) shows active notes

Enjoy your QWERTY MIDI Keyboard!
