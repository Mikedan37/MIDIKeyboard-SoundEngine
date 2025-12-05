# Platform Compatibility

## Supported Platforms

### macOS
- **Status**: Fully supported
- **Setup**: `./setup.sh`
- **Features**: All features including menu bar GUI
- **Auto-launch**: LaunchAgent (configured automatically)
- **Requirements**: Python 3.9+, macOS 10.14+

### Windows
- **Status**: Fully supported
- **Setup**: `.\setup.ps1`
- **Features**: All features except menu bar GUI (runs in background)
- **Auto-launch**: Task Scheduler (configured automatically)
- **Requirements**: Python 3.9+, Windows 10+
- **Note**: May need to adjust PowerShell execution policy

### Linux
- **Status**: Fully supported
- **Setup**: `./setup.sh`
- **Features**: All features except menu bar GUI (runs in background)
- **Auto-launch**: systemd user service (configured automatically)
- **Requirements**: Python 3.9+, systemd

## What Works on Each Platform

| Feature | macOS | Windows | Linux |
|---------|-------|---------|-------|
| Audio synthesis | Yes | Yes | Yes |
| MIDI input | Yes | Yes | Yes |
| QWERTY keyboard input | Yes | Yes | Yes |
| Menu bar GUI | Yes | No | No |
| Auto-launch | Yes | Yes | Yes |
| Pico detection | Yes | Yes | Yes |

## Setup Scripts

### macOS / Linux
```bash
./setup.sh
```

Automatically:
- Detects OS
- Installs Python dependencies
- Sets up auto-launch (LaunchAgent on macOS, systemd on Linux)
- Configures monitoring

### Windows
```powershell
.\setup.ps1
```

Automatically:
- Installs Python dependencies
- Sets up Task Scheduler for auto-launch
- Configures monitoring

## Known Issues & Solutions

### Windows PowerShell Execution Policy

If setup fails with execution policy error:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Then run `.\setup.ps1` again.

### Linux: Missing lsusb

If Pico detection fails on Linux:

```bash
sudo apt-get install usbutils # Debian/Ubuntu
# or
sudo yum install usbutils # RHEL/CentOS
```

### macOS: Menu Bar Not Appearing

If menu bar doesn't appear:
- Check that `rumps` is installed: `pip install rumps`
- Verify LaunchAgent is running: `launchctl list | grep qwertymidi`

## Testing

All platforms tested with:
- Python 3.9+
- Standard USB MIDI devices
- Pico firmware

## Platform-Specific Notes

### macOS
- Menu bar GUI requires `rumps` (installed automatically)
- Uses `system_profiler` for Pico detection
- LaunchAgent runs on login

### Windows
- No GUI - runs in background mode
- Uses PowerShell for Pico detection
- Task Scheduler runs on login
- May require admin privileges for Task Scheduler

### Linux
- No GUI - runs in background mode
- Uses `lsusb` for Pico detection (requires usbutils)
- systemd service runs on login
- Requires systemd (most modern Linux distributions)

## Verification

After setup, verify on each platform:

### macOS
```bash
launchctl list | grep qwertymidi
tail -f ~/Library/Logs/qwerty_midi.log
```

### Windows
```powershell
Get-ScheduledTask -TaskName QWERTY_MIDI_Keyboard
```

### Linux
```bash
systemctl --user status qwerty-midi.service
journalctl --user -u qwerty-midi.service -f
```

