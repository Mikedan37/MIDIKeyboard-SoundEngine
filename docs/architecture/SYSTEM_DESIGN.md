# QWERTY & MIDI Keyboard System - Complete System Design & Architecture

## Table of Contents
1. [System Overview](#system-overview)
2. [Architecture Diagram](#architecture-diagram)
3. [Component Breakdown](#component-breakdown)
4. [Data Flow Analysis](#data-flow-analysis)
5. [Communication Protocols](#communication-protocols)
6. [Threading Model](#threading-model)
7. [Hardware Integration](#hardware-integration)
8. [Software Architecture](#software-architecture)
9. [Design Patterns](#design-patterns)
10. [System Interactions](#system-interactions)

---

## System Overview

This is a **dual-mode USB keyboard system** that functions as both:
- **Standard QWERTY Keyboard** (USB HID)
- **MIDI Musical Instrument** (USB MIDI)

The system consists of:
- **Hardware Layer**: Raspberry Pi Pico (RP2040) with GPIO key inputs
- **Firmware Layer**: C-based embedded firmware with TinyUSB
- **Software Layer**: Python-based audio engine and macOS GUI
- **Communication Layer**: USB (HID + MIDI) and Serial protocols

---

## Architecture Diagram

```

 PHYSICAL HARDWARE LAYER 

 GPIO Pin 0 GPIO Pin 1 GPIO Pin 2 ... 
 (Key 1) (Key 2) (Key 3) 

 Raspberry Pi Pico 
 (RP2040) 
 Dual-Core ARM M0+ 

 USB Connection

 FIRMWARE LAYER (C) 

 CORE 0: Main Firmware 

 main.c 
 USB Task Loop (tud_task) 
 Key State Management 
 MIDI Note Sending (send_midi_note) 
 HID Key Sending (send_hid_key) 

 usb_descriptors.c 
 USB Device Descriptor 
 HID Report Descriptor 
 MIDI Class Descriptor 
 String Descriptors 

 CORE 1: GPIO Polling 

 drivers/gpio_driver.c 
 gpio_driver_init() 
 gpio_poll_loop() [Background Thread] 
 gpio_is_pressed() [State Query] 

 TinyUSB Library 
 USB HID Class Implementation 
 USB MIDI Class Implementation 
 USB Device Management 

 USB HID USB MIDI

 macOS Host System macOS Host System 

 USB HID Driver USB MIDI Driver 
 (System Level) (System Level) 

 Keyboard Input MIDI Input 
 (Standard Typing) (Musical Notes) 

 SOFTWARE LAYER (Python) 

 MAIN THREAD: Audio Engine 

 engine.py 
 play_note(note, velocity) 
 stop_note(note) 
 audio_callback() [Real-time audio generation] 
 start_audio_engine() 
 Thread-safe note state management 

 synth_menu.py (macOS Menu Bar) 
 SynthMenuBarApp class 
 Real-time note display 
 100ms refresh timer 

 BACKGROUND THREADS: Input Listeners 

 Thread 1: MIDI Listener 

 pico_listener.py 
 mido.open_input() [USB MIDI] 
 Auto-reconnect on disconnect 
 Calls engine.play_note() / stop_note() 

 Thread 2: Serial MIDI Bridge 

 serial_midi_adapter.py 
 Serial port detection (usbmodem) 
 Protocol: "ON:60" / "OFF:60" 
 Calls engine.play_note() / stop_note() 

 Thread 3: Keyboard Listener 

 mac_keyboard_listener.py 
 pynput.keyboard.Listener 
 QWERTY → MIDI mapping (midi_utils.py) 
 Mario theme playback 

 Orchestration Layer 

 monitor_and_launch.py 
 start_audio_engine() [Main thread] 
 launch_listeners() [Background threads] 
 SynthMenuBarApp().run() [GUI main loop] 

 unified_listener.py 
 Launches all input listener threads 
 Thread management 

 AUDIO OUTPUT 

 sounddevice (PortAudio) 
 Sample Rate: 44.1 kHz 
 Block Size: 512 samples 
 Channels: Mono (1) 
 Format: Float32 

 macOS Audio System 
 Hardware Audio Output (Speakers/Headphones) 

```

---

## Component Breakdown

### 1. Hardware Layer

#### Raspberry Pi Pico (RP2040)
- **Microcontroller**: Dual-core ARM Cortex-M0+
- **GPIO Pins Used**: 5 pins (GPIO 0-4) for key inputs
- **USB**: Native USB support for HID and MIDI
- **Memory**: 264KB RAM, 2MB Flash

#### Key Mapping
```
GPIO Pin 0 → Key 1 → MIDI Note 60 (C4) → HID Keycode 0x04 (A)
GPIO Pin 1 → Key 2 → MIDI Note 62 (D4) → HID Keycode 0x05 (B)
GPIO Pin 2 → Key 3 → MIDI Note 64 (E4) → HID Keycode 0x06 (C)
GPIO Pin 3 → Key 4 → MIDI Note 65 (F4) → HID Keycode 0x07 (D)
GPIO Pin 4 → Key 5 → MIDI Note 67 (G4) → HID Keycode 0x08 (E)
```

### 2. Firmware Layer

#### Core 0: Main Firmware (`main.c`)
**Responsibilities:**
- USB task management (`tud_task()`)
- Key state polling from Core 1
- MIDI message generation and transmission
- HID keyboard report generation
- Dual-mode operation coordination

**Key Functions:**
```c
void send_midi_note(uint8_t note, bool on)
 - Generates MIDI note_on (0x90) or note_off (0x80) messages
 - Uses tud_midi_stream_write() for transmission

void send_hid_key(uint8_t keycode)
 - Generates HID keyboard reports
 - Sends key press and release events
```

#### Core 1: GPIO Polling (`drivers/gpio_driver.c`)
**Responsibilities:**
- Continuous GPIO state polling (5ms intervals)
- Debouncing and state change detection
- Key state storage for main core queries

**Key Functions:**
```c
void gpio_poll_loop()
 - Runs on Core 1 as background task
 - Polls all GPIO pins continuously
 - Updates key_state array

bool gpio_is_pressed(uint8_t pin)
 - Query function for main core
 - Returns current key state
```

#### USB Descriptors (`usb_descriptors.c`)
**USB Device Configuration:**
- **Vendor ID**: 0xCafe
- **Product ID**: 0x4000
- **Device Class**: MISC (Composite Device)
- **Interfaces**: HID (Keyboard) + MIDI

**Endpoints:**
- **EP 0x81**: HID IN (Keyboard reports)
- **EP 0x02**: MIDI OUT (Note messages to host)
- **EP 0x83**: MIDI IN (Note messages from host)

### 3. Software Layer

#### Audio Engine (`engine.py`)
**Core Audio Processing:**
- **Sample Rate**: 44,100 Hz (CD quality)
- **Block Size**: 512 samples (~11.6ms latency)
- **Polyphony**: Unlimited (set-based management)
- **Waveform**: Sine wave generation
- **Sustain**: 0.3 second timeout-based release

**Thread Safety:**
- Uses `threading.Lock()` for shared state
- Thread-safe note addition/removal
- Atomic state updates

**Key Data Structures:**
```python
held_notes = set() # Active MIDI notes
note_timestamps = {} # Note start times
phase_dict = {} # Phase tracking per note
lock = threading.Lock() # State synchronization
```

#### Input Listeners

**1. MIDI Listener (`pico_listener.py`)**
- Uses `mido` library for USB MIDI communication
- Auto-detects Pico MIDI device
- Auto-reconnects on disconnect
- Parses MIDI messages (note_on/note_off)

**2. Serial MIDI Bridge (`serial_midi_adapter.py`)**
- Detects Pico serial port (usbmodem)
- Protocol: `"ON:60"` / `"OFF:60"` (text-based)
- Baud rate: 115200
- Fallback communication method

**3. Keyboard Listener (`mac_keyboard_listener.py`)**
- Uses `pynput` for low-level keyboard capture
- Maps QWERTY keys to MIDI notes
- Includes Mario theme melody playback
- Thread-safe note sequencing

#### macOS GUI (`synth_menu.py`)
- **Framework**: `rumps` (macOS menu bar library)
- **Update Rate**: 100ms (10 Hz)
- **Display**: Current note number and frequency
- **Controls**: Stop synth, Exit

#### Orchestration (`monitor_and_launch.py`)
- **Entry Point**: Main application launcher
- **Initialization Order**:
 1. Start audio engine (main thread)
 2. Launch background listeners
 3. Start menu bar GUI
- **Lifecycle Management**: Graceful shutdown handling

---

## Data Flow Analysis

### Path 1: Physical Key → Audio Output

```
1. Physical Key Press

2. GPIO Pin State Change (Hardware)

3. Core 1 GPIO Polling (5ms interval)

4. Key State Update (gpio_is_pressed())

5. Core 0 Main Loop Detection

 send_midi_note(60, true) USB MIDI macOS MIDI Driver

 send_hid_key(0x04) USB HID macOS Keyboard Input

6. Python MIDI Listener (pico_listener.py)

7. engine.play_note(60)

8. Audio Callback (audio_callback)

 Sine wave generation (NumPy)
 Phase accumulation
 Buffer normalization

9. sounddevice Output Stream

10. macOS Audio System

11. Hardware Audio Output (Speakers)
```

### Path 2: QWERTY Keyboard → Audio Output

```
1. User Types Key (e.g., 'a')

2. macOS Keyboard Input

3. mac_keyboard_listener.py (pynput)

4. midi_utils.key_to_midi('a') → 60

5. engine.play_note(60)

6. Audio Callback → Audio Output
 (Same as Path 1, steps 8-11)
```

### Path 3: Serial Communication → Audio Output

```
1. Pico Serial Output: "ON:60\n"

2. serial_midi_adapter.py

 Port detection (usbmodem)
 Serial read (115200 baud)
 Protocol parsing

3. engine.play_note(60)

4. Audio Callback → Audio Output
 (Same as Path 1, steps 8-11)
```

---

## Communication Protocols

### 1. USB HID (Human Interface Device)

**Purpose**: Standard keyboard input

**Message Format**:
```
HID Keyboard Report (6 bytes):
[0]: Keycode (0x04 = A, 0x05 = B, etc.)
[1-5]: Reserved/Additional keys
```

**Transmission**:
- Key Press: `tud_hid_keyboard_report(0, 0, report)`
- Key Release: `tud_hid_keyboard_report(0, 0, NULL)`

### 2. USB MIDI

**Purpose**: Musical note communication

**Message Format**:
```
MIDI Message (3 bytes):
[0]: Status byte (0x90 = note_on, 0x80 = note_off)
[1]: Note number (0-127, 60 = C4)
[2]: Velocity (0-127, 100 = default)
```

**Transmission**:
- `tud_midi_stream_write(0, msg, 3)`

### 3. Serial Protocol

**Purpose**: Alternative communication method

**Format**: Text-based, line-delimited
```
ON:60 # Note on, MIDI note 60
OFF:60 # Note off, MIDI note 60
```

**Settings**:
- Baud Rate: 115200
- Data Bits: 8
- Stop Bits: 1
- Parity: None

---

## 🧵 Threading Model

### Thread Architecture

```

 PROCESS SPACE 

 MAIN THREAD (Python) 
 monitor_and_launch.py 
 engine.start_audio_engine() 
 SynthMenuBarApp().run() [GUI Loop] 
 Audio callback execution (sounddevice) 

 BACKGROUND THREAD 1: MIDI Listener 
 pico_listener.midi_listener() 
 mido.open_input() [Blocking] 
 Auto-reconnect loop 

 BACKGROUND THREAD 2: Serial Bridge 
 serial_midi_adapter.serial_to_midi_bridge() 
 Serial port reading [Blocking] 
 Protocol parsing 

 BACKGROUND THREAD 3: Keyboard Listener 
 mac_keyboard_listener.start_keyboard_...() 
 pynput.keyboard.Listener [Event-driven] 
 Mario note playback threads 

 BACKGROUND THREAD 4: Audio Engine 
 engine._audio_loop() 
 sounddevice.OutputStream callback 

```

### Thread Synchronization

**Shared State Protection:**
```python
# engine.py
lock = threading.Lock() # Protects:
held_notes = set() # - Active notes
note_timestamps = {} # - Note timing
phase_dict = {} # - Phase state
last_note = None # - Display state
last_freq = None # - Display state
```

**Critical Sections:**
- `play_note()`: Acquires lock, modifies shared state
- `stop_note()`: Acquires lock, modifies shared state
- `audio_callback()`: Acquires lock, reads shared state
- `get_last_note_info()`: Acquires lock, reads display state

---

## Hardware Integration

### GPIO Configuration

**Pin Setup:**
```c
for (int i = 0; i < NUM_KEYS; ++i) {
 gpio_init(gpio_key_pins[i]); // Initialize pin
 gpio_set_dir(gpio_key_pins[i], GPIO_IN); // Input mode
 gpio_pull_down(gpio_key_pins[i]); // Pull-down resistor
}
```

**State Detection:**
- **Low (0)**: Key not pressed (pulled down)
- **High (1)**: Key pressed (connected to VCC)

### Multicore Architecture

**Core 0 (Main Core):**
- USB task management
- Key state processing
- MIDI/HID message generation
- Main application loop

**Core 1 (Secondary Core):**
- GPIO polling loop
- Continuous key scanning
- State change detection
- Non-blocking operation

**Inter-Core Communication:**
- Shared memory (key_state array)
- Function call: `multicore_launch_function()`
- No explicit synchronization needed (read-only from Core 0)

---

## Software Architecture

### Module Dependencies

```
monitor_and_launch.py
 engine.py
 numpy
 sounddevice
 unified_listener.py
 pico_listener.py
 mido
 engine.py
 mac_keyboard_listener.py
 pynput
 engine.py
 midi_utils.py
 serial_midi_adapter.py
 serial
 engine.py
 synth_menu.py
 rumps
 engine.py
```

### Design Patterns

**1. Producer-Consumer Pattern**
- **Producers**: Input listeners (MIDI, Serial, Keyboard)
- **Consumer**: Audio engine callback
- **Queue**: Shared state (held_notes set)

**2. Observer Pattern**
- **Subject**: Audio engine state
- **Observer**: Menu bar GUI (updates every 100ms)

**3. Strategy Pattern**
- **Context**: Audio engine
- **Strategies**: Different input sources (MIDI, Serial, Keyboard)
- **Interface**: `play_note()` / `stop_note()`

**4. Singleton Pattern**
- **Instance**: Audio engine (single global state)
- **Access**: Module-level functions

**5. Thread Pool Pattern**
- **Pool**: Background listener threads
- **Management**: unified_listener.py

---

## System Interactions

### Startup Sequence

```
1. User plugs in Pico

2. macOS detects USB device (HID + MIDI)

3. User runs monitor_and_launch.py

 start_audio_engine()
 Initialize sounddevice
 Start audio callback thread
 Begin real-time synthesis

 launch_listeners()
 Start MIDI listener thread
 Start serial bridge thread
 Start keyboard listener thread

 SynthMenuBarApp().run()
 Start menu bar GUI loop
```

### Runtime Operation

```

 Continuous Operation Loop 

 Input Sources (Parallel) 
 Physical Keys → GPIO → MIDI/HID 
 QWERTY Keys → Keyboard Listener 
 Serial Port → Serial Bridge 

 Audio Engine (Thread-Safe) 
 play_note() / stop_note() 
 State management (lock) 
 Audio callback (real-time) 

 Output (Parallel) 
 Audio → Speakers 
 GUI → Menu Bar (100ms updates) 

```

### Shutdown Sequence

```
1. User clicks "Stop Synth" or "Exit"

2. SynthMenuBarApp.quit_app()

 engine.shutdown()
 Set playing = False

 rumps.quit_application()
 Exit GUI loop

 Background threads terminate (daemon threads)
```

---

## Design Decisions & Rationale

### 1. Dual-Core Architecture
**Decision**: Use Core 1 for GPIO polling
**Rationale**: 
- Non-blocking GPIO scanning
- Main core free for USB tasks
- Better real-time performance

### 2. Thread-Safe Audio Engine
**Decision**: Use threading.Lock() for state
**Rationale**:
- Multiple input sources accessing shared state
- Real-time audio callback needs safe access
- Prevents race conditions

### 3. Multiple Input Methods
**Decision**: Support MIDI, Serial, and Keyboard
**Rationale**:
- Redundancy and flexibility
- Different use cases (hardware vs software)
- Development and debugging support

### 4. Timeout-Based Note Release
**Decision**: 0.3 second auto-release
**Rationale**:
- Handles missing note_off messages
- Prevents stuck notes
- Natural decay behavior

### 5. Menu Bar GUI
**Decision**: Use rumps for lightweight GUI
**Rationale**:
- Native macOS integration
- Minimal resource usage
- Non-intrusive interface

---

## System Limitations & Future Improvements

### Current Limitations
1. **Fixed Key Mapping**: Hardcoded GPIO-to-MIDI mapping
2. **Simple Synthesis**: Sine waves only
3. **No Velocity Sensitivity**: Fixed velocity (100)
4. **Limited Polyphony Management**: No voice stealing
5. **macOS Only**: GUI limited to macOS

### Recommended Improvements
1. **Configurable Key Mapping**: User-defined MIDI assignments
2. **Advanced Synthesis**: Multiple waveforms, filters, effects
3. **Velocity Support**: Hardware velocity detection
4. **Voice Management**: Priority-based polyphony
5. **Cross-Platform GUI**: Windows/Linux support
6. **AI Integration**: Note prediction (ai_predict.py exists but minimal)

---

## Summary

This system demonstrates a **sophisticated embedded-to-software architecture** with:

- **Hardware Layer**: Raspberry Pi Pico with GPIO key inputs
- **Firmware Layer**: Dual-core C firmware with USB HID/MIDI
- **Software Layer**: Multi-threaded Python audio engine
- **Communication**: USB (HID + MIDI) and Serial protocols
- **Real-Time Processing**: Low-latency audio synthesis
- **User Interface**: Native macOS menu bar integration

The architecture successfully bridges **embedded systems** and **desktop software** to create a functional musical instrument that doubles as a standard keyboard.

