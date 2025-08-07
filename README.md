# QWERTY & MIDI Plug-and-Play Keyboard System

[![Build](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/yourusername/repo/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico-orange)]()
[![Language](https://img.shields.io/badge/language-C%20%7C%20Python%20%7C%20Assembly-lightgrey)]()

Turn any computer into a full-blown musical instrument.  
A Senior Design project fusing a standard QWERTY keyboard and USB MIDI into a dynamic performance tool—powered by the Raspberry Pi Pico.

> Just plug it in. Start typing. Hear music.

---

## Project Overview

- **Title**: QWERTY & MIDI Plug-and-Play Keyboards with Driver Framework  
- **Team**:  
  - Michael Danylchuk — Firmware, Audio Engine, macOS Synth GUI  
  - Christopher “Zac” Hatchett — Hardware Design, PCB, GPIO Integration  
- **Platform**: Raspberry Pi Pico (RP2040)  
- **Languages**: C, Python (macOS), Assembly (USB descriptors)

---

## Features

### Dual-Mode Keyboard
- QWERTY Mode: Functions like a normal keyboard.
- MIDI Mode: Each key maps to a musical note.
- Live-switchable modes via custom C firmware.

### Real-Time Sound Engine
- Synth written in pure Python.
- Supports polyphonic playback (chords and overlays).
- Natural release via timeout-based sustain.

### macOS Menu Bar App
- Built using `rumps`.
- Live display of MIDI note and frequency.
- Instant updates on active keypresses.

### Modular Engine Architecture
- Threaded design: QWERTY input, Pico MIDI input, and GUI all run concurrently.
- Real-time communication between `engine.py` and the menu bar.
- Autodetects connected Pico MIDI device.

---

## Folder Structure

```
midi_sound_engine/
├── engine.py               # Core synth logic
├── test_play.py            # QWERTY polling + MIDI input test
├── monitor_and_launch.py   # Auto-launch on Pico plug-in
├── synth_menu.py           # macOS menu bar GUI
└── pico_listener.py        # Reads MIDI notes from the Pico

qwerty_midi_pico/
├── drivers/                # Custom GPIO/MIDI drivers
├── usb_descriptors.c/h     # USB MIDI class descriptors
├── tonegen.c               # Test tone generation
└── main.c                  # Firmware entry point
```

---

## How It Works

1. **Plug in the Pico**  
   Shows up as a USB MIDI device.

2. **Auto-Detect & Launch**  
   `monitor_and_launch.py`:
   - Starts the synth engine.
   - Launches the menu bar GUI.
   - Begins input listening.

3. **Start Typing**  
   - Keys `a–k` play notes.  
   - Hold multiple keys to play chords.  
   - Watch the menu bar for real-time feedback.

---

## Dependencies

### Python (macOS Synth Engine)
Install via `pip`:

```bash
pip install sounddevice numpy rumps mido python-rtmidi keyboard
```

### C (Pico Firmware)
- [TinyUSB](https://github.com/hathach/tinyusb)
- CMake + pico-sdk toolchain

---

## Educational Objectives

This project demonstrates:

- USB MIDI driver development
- Embedded firmware design
- Real-time audio synthesis in Python
- Threaded architecture with live UI updates
- macOS GUI development using rumps
- End-to-end embedded systems integration

---

## Demo

Coming Soon: Full video walkthrough and live sound demo.

---

## Future Improvements

- Waveform selection & EQ support  
- Audio input-to-note pitch detection  
- Windows & Linux GUI ports  
- AI-based note prediction (via `ai_predict.py`)

---

## Credits

San Jose State University – EE198A  
Mentor: Dr. Nadir Mir  
Custom hardware + firmware developed by Michael Danylchuk & Zac Hatchett

---

Built with love, solder, and Python.

