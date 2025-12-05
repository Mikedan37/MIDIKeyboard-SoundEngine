# MIDI Sound Engine

Real-time polyphonic synthesizer with macOS menu bar integration.

## Features

- Real-time audio synthesis
- Polyphonic playback (multiple keys simultaneously)
- macOS menu bar GUI
- Automatic Pico detection
- MIDI input support

## Setup

```bash
python3 -m venv venv
source venv/bin/activate # On macOS/Linux
pip install -r requirements.txt
```

## Structure

```
midi_sound_engine/
 engine.py # Core synthesis engine
 synth_menu.py # macOS menu bar GUI
 pico_listener.py # MIDI input handler
 monitor_and_launch.py # Auto-launcher
 unified_listener.py # Unified input handler
 requirements.txt # Python dependencies
```

## Usage

### Manual Launch
```bash
python engine.py
```

### Auto-Launch (macOS)
```bash
./setup_pico_autorun.sh
```

This sets up automatic launching when the Pico is connected.

## Documentation

See [../docs/](../docs/) for complete documentation.
