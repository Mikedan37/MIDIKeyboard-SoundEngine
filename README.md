🎹 QWERTY & MIDI Plug-and-Play Keyboard System

Turn any computer into a full-blown musical instrument.
This Senior Design project is a custom-built QWERTY + MIDI USB keyboard that supports polyphonic playback, live synthesizer control, and driver-level customization using the Raspberry Pi Pico.
 
🔌 Just plug it in. Start typing. Hear music.

⸻

🔧 Project Overview

Title: QWERTY & MIDI Plug-and-Play Keyboards with Driver Framework
Team Members:
	•	👨‍💻 Michael Danylchuk — Firmware, Audio Engine, macOS Synth GUI
	•	🔩 Christopher “Zac” Hatchett — Hardware Design, PCB, GPIO Integration
Platform: Raspberry Pi Pico (RP2040)
Languages: C, Python (macOS), Assembly (USB descriptors)

⸻

🚀 Features

🎼 QWERTY + MIDI Keyboard Modes
	•	QWERTY Mode: Sends typed characters like a normal USB keyboard.
	•	MIDI Mode: Each key maps to a MIDI note.
	•	Seamlessly switchable firmware with custom C driver code.

🔊 Real-Time Sound Engine
	•	Pure Python synth with live playback via sounddevice.
	•	Polyphonic: Hold and mix multiple keys at once.
	•	Timeout-based sustain for natural release.

🍎 macOS Menu Bar App
	•	Lightweight menu extra using rumps.
	•	Displays the live MIDI note and frequency.
	•	Smooth status updates on currently played tones.

🧠 Smart Engine Architecture
	•	Modular threading: keyboard input, Pico MIDI, and GUI run concurrently.
	•	Real-time updates from engine.py sync with the menu bar GUI.
	•	MIDI listener automatically detects and connects to the Pico.

⸻

🛠️ Folder Structure

├── midi_sound_engine/
│   ├── engine.py              # Synthesizer engine
│   ├── test_play.py           # QWERTY polling + MIDI support
│   ├── monitor_and_launch.py  # Auto-launcher when Pico is plugged in
│   ├── synth_menu.py          # macOS menu bar interface
│   └── pico_listener.py       # Reads MIDI notes from Pico
│
├── qwerty_midi_pico/
│   ├── drivers/               # Custom C driver for GPIO and key mapping
│   ├── usb_descriptors.c/h    # MIDI class descriptors
│   ├── tonegen.c              # Tone generator logic (for microcontroller test)
│   └── main.c                 # Entry point for firmware



⸻

💻 How It Works

Step 1: Plug It In

The Pico shows up as a USB MIDI device.

Step 2: Autodetection

The monitor_and_launch.py script detects the Pico and:
	•	Starts the Python synth engine.
	•	Launches the macOS menu bar GUI.
	•	Begins listening for input from both your QWERTY and Pico.

Step 3: Start Typing
	•	Press a–k to play notes.
	•	Multiple keys can be held to create chords.
	•	Menu bar updates in real-time with note + frequency.

⸻

📦 Dependencies

Python (macOS Synth Engine):

pip install sounddevice numpy rumps mido python-rtmidi keyboard

C (Pico Firmware):
	•	TinyUSB MIDI class
	•	CMake toolchain
	•	Tested with pico-sdk

⸻

🎓 Educational Goals

This project teaches:
	•	USB driver development
	•	Real-time audio synthesis
	•	Multi-threaded design
	•	Embedded systems integration
	•	Cross-platform MIDI support
	•	GUI development for embedded interfaces

⸻

📽️ Demo

Coming Soon: Full walkthrough video + live sound demo.

⸻

🧠 Future Improvements
	•	🎛️ Add EQ, waveform shape selection
	•	🎤 Input-based pitch detection
	•	💻 Windows/Linux support for GUI
	•	🧪 AI-based note prediction via ai_predict.py

⸻

🤝 Credits

Created at San Jose State University (EE198A)
Mentored by Dr. Nadir Mir

⸻

