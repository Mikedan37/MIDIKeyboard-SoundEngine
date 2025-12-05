# Flashing Firmware to Pico

Guide for flashing the QWERTY MIDI Keyboard firmware to your Raspberry Pi Pico.

## Quick Flash (Pre-built)

If you have a pre-built `.uf2` file:

1. **Hold BOOTSEL** button on your Pico
2. **Connect** Pico to computer via USB
3. **Release** BOOTSEL button
4. **Copy** the `.uf2` file to the mounted `RPI-RP2` drive
5. Pico will **automatically reboot** with new firmware

## Build and Flash

### Prerequisites

- CMake 3.13+
- ARM GCC toolchain (included with Pico SDK)
- Pico SDK

### Build Steps

```bash
cd qwerty_midi_pico
mkdir build && cd build
cmake ..
make
```

This creates `qwerty_midi_pico.uf2` in the `build/` directory.

### Flash Steps

1. Hold **BOOTSEL** on Pico
2. Connect via USB
3. Copy `qwerty_midi_pico.uf2` to `RPI-RP2` drive
4. Pico reboots automatically

## Using picotool (Advanced)

If you have `picotool` installed:

```bash
# Build first (see above)
cd build

# Flash
picotool load qwerty_midi_pico.uf2

# Or flash and reboot
picotool load -f qwerty_midi_pico.uf2
```

## Verification

After flashing:

1. **Check USB device**:
 - macOS: `system_profiler SPUSBDataType | grep -i pico`
 - Linux: `lsusb | grep -i pico`
 - Windows: Device Manager → Universal Serial Bus devices

2. **Check MIDI device**:
 - macOS: `system_profiler SPUSBDataType | grep -i midi`
 - Linux: `amidi -l`
 - Windows: Device Manager → Sound, video and game controllers

3. **Test with monitor script**:
 ```bash
 cd midi_sound_engine
 source venv/bin/activate
 python monitor_and_launch.py
 ```

## Troubleshooting

### Pico Not Appearing as Drive

- **Try different USB cable** (data cable, not charge-only)
- **Try different USB port**
- **Hold BOOTSEL longer** (2-3 seconds)
- **Check Pico LED** (should blink when in bootloader mode)

### Build Errors

- **CMake not found**: Install CMake 3.13+
- **Pico SDK not found**: Set `PICO_SDK_PATH` environment variable
- **Toolchain not found**: Install ARM GCC toolchain

### Firmware Not Working

- **Check pin assignments**: Update in `drivers/current/velocity_matrix.h`
- **Check USB connection**: Verify Pico is powered
- **Check logs**: Monitor script will show errors

## Configuration

Before flashing, update pin assignments in:

- `drivers/current/velocity_matrix.h` - Row pins
- `drivers/current/msqt32_shift_register.h` - Shift register pins

See [README.md](README.md) for configuration details.

## Updating Firmware

To update firmware:

1. Build new firmware (see Build Steps)
2. Flash using same method as initial flash
3. Pico will automatically use new firmware

No need to erase or reset - just flash the new `.uf2` file.

