# Installation Guide

Complete installation guide for the QWERTY MIDI Keyboard.

## Quick Start

### macOS / Linux

```bash
chmod +x setup.sh
./setup.sh
```

### Windows

```powershell
.\setup.ps1
```

The setup script automatically installs dependencies and configures auto-launch.

## Prerequisites

### Required
- **Python 3.9+** - [Download Python](https://www.python.org/downloads/)
- **Raspberry Pi Pico** - Hardware device

### Optional (for firmware development)
- **CMake 3.13+** - [Download CMake](https://cmake.org/download/)
- **Pico SDK** - [Get Pico SDK](https://github.com/raspberrypi/pico-sdk)

## Manual Setup

If you prefer manual setup:

### 1. Python Environment

```bash
cd midi_sound_engine
python3 -m venv venv
source venv/bin/activate # On Windows: venv\Scripts\Activate.ps1
pip install -r requirements.txt
```

### 2. Auto-Launch Setup

#### macOS
The setup script creates a LaunchAgent. To set up manually:

```bash
# Copy the plist file
cp .github/launch_agents/com.qwertymidi.pico.plist ~/Library/LaunchAgents/

# Load it
launchctl load ~/Library/LaunchAgents/com.qwertymidi.pico.plist
```

#### Linux
The setup script creates a systemd user service. To set up manually:

```bash
# Copy the service file
cp .github/systemd/qwerty-midi.service ~/.config/systemd/user/

# Enable and start
systemctl --user daemon-reload
systemctl --user enable qwerty-midi.service
systemctl --user start qwerty-midi.service
```

#### Windows
The setup script creates a Task Scheduler task. To set up manually, use Task Scheduler GUI or see `setup.ps1` for PowerShell commands.

## Flashing Firmware

### Option 1: Pre-built Firmware

If you have a pre-built `.uf2` file:

1. Hold **BOOTSEL** button on Pico
2. Connect Pico via USB
3. Copy `.uf2` file to the mounted drive
4. Pico will reboot with new firmware

### Option 2: Build from Source

```bash
cd qwerty_midi_pico
mkdir build && cd build
cmake ..
make

# Flash the .uf2 file (see Option 1)
```

See [qwerty_midi_pico/FLASH.md](qwerty_midi_pico/FLASH.md) for detailed flashing instructions.

## Verification

After setup:

1. **Check Python environment**:
 ```bash
 cd midi_sound_engine
 source venv/bin/activate
 python --version
 ```

2. **Test synthesizer**:
 ```bash
 python engine.py
 ```

3. **Plug in Pico**:
 - Should appear as USB MIDI device
 - Monitor script should detect it
 - Synthesizer should auto-launch

## Troubleshooting

### Python Issues
- **"python3: command not found"**: Install Python 3.9+
- **"pip: command not found"**: Run `python -m ensurepip`

### Auto-Launch Issues

#### macOS
```bash
# Check if LaunchAgent is loaded
launchctl list | grep qwertymidi

# View logs
tail -f ~/Library/Logs/qwerty_midi.log
```

#### Linux
```bash
# Check service status
systemctl --user status qwerty-midi.service

# View logs
journalctl --user -u qwerty-midi.service -f
```

#### Windows
```powershell
# Check task status
Get-ScheduledTask -TaskName QWERTY_MIDI_Keyboard

# View task history
Get-WinEvent -LogName Microsoft-Windows-TaskScheduler/Operational | Where-Object {$_.Message -like "*QWERTY*"}
```

### Pico Not Detected
- Check USB connection
- Verify firmware is flashed
- Check device manager (Windows) or `lsusb` (Linux/Mac)

## Next Steps

- Read [README.md](README.md) for project overview
- See [QUICKSTART.md](QUICKSTART.md) for quick setup
- Check [docs/](docs/) for complete documentation

## Need Help?

- Open an [issue](.github/ISSUE_TEMPLATE/bug_report.md)
- Check [documentation](docs/)
- Review troubleshooting section above
