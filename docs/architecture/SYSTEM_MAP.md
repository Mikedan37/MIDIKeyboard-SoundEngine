# Complete System Map & Component Relationships

## Visual System Map

```

 MIDI KEYBOARD SYSTEM

 PHYSICAL LAYER 

 [Key 1] [Key 2] [Key 3] [Key 4] [Key 5] 

 Raspberry Pi Pico 
 (RP2040) 

 Core 0 Main Firmware 
 main.c USB Tasks 
 Key Processing 
 Message Generation 
 TinyUSB 

 Core 1 GPIO Polling 
 gpio_driver.c Continuous Scanning 
 State Management 

 USB Cable 

 COMMUNICATION LAYER 

 USB MIDI Interface 

 Endpoint: 0x02 (OUT) 
 Endpoint: 0x83 (IN) 
 Protocol: MIDI 

 macOS MIDI Driver 
 (System Level) 

 MIDI Message Stream 
 (Musical Notes) 

 APPLICATION LAYER (Python) 

 INPUT LAYER 

 MIDI Serial 
 Listener Bridge 

 pico_listener serial_midi_ 
 .py adapter.py 

 Thread 1 Thread 2 

 unified_listener.py 
 (Thread Manager) 

 CORE ENGINE 

 engine.py 

 State Management (Thread-Safe) 
 held_notes: set() 
 note_timestamps: dict() 
 phase_dict: dict() 
 lock: threading.Lock() 

 Audio Synthesis 
 play_note(note, velocity) 
 stop_note(note) 
 audio_callback() [Real-time] 
 Sine wave generation (NumPy) 

 OUTPUT LAYER 

 Audio GUI 
 Output Display 

 sounddevice synth_menu.py 
 (PortAudio) (rumps) 

 44.1kHz Menu Bar 
 512 samples 100ms update 

 macOS Audio System macOS Menu Bar 
 (CoreAudio) (System UI) 

 Hardware Audio Output 
 (Speakers/Headphones) 

```

## Component Interaction Matrix

```
 GPIO USB MIDI Serial Audio GUI 

GPIO Driver - - - - - - 
USB MIDI - - - - 
Serial Bridge - - - - - 
MIDI Listener - - - - - 
Audio Engine - - - - - 
GUI - - - - - 
```

## Data Flow Summary

### Input Paths (2 total)

**Path 1: Physical Keys**
```
Physical Key → GPIO → Core 1 → Core 0 → USB MIDI → macOS → Python → Audio
```

**Path 2: Serial Port**
```
Pico Serial → macOS Serial → Python → Audio
```

### Output Paths (2 total)

**Path 1: Audio**
```
Python Engine → sounddevice → macOS Audio → Hardware
```

**Path 2: GUI**
```
Python Engine → rumps → macOS Menu Bar → Display
```

## Thread Execution Flow

```

 PROCESS TIMELINE 

 T0: System Startup 

 Main Thread: Initialize Audio Engine 
 Start Audio Callback Thread 

 Main Thread: Launch Input Listeners 
 Thread 1: MIDI Listener (daemon) 
 Thread 2: Serial Bridge (daemon) 

 Main Thread: Start GUI Loop 
 Menu Bar App (blocking) 

 T1: Runtime Operation 

 Background Threads: Continuous I/O 
 MIDI: Blocking read, auto-reconnect 
 Serial: Blocking read, error handling 

 Audio Thread: Real-time Callback 
 86 Hz callback rate (512 samples @ 44.1kHz) 

 Main Thread: GUI Updates 
 10 Hz update rate (100ms intervals) 

 T2: Shutdown 

 User Action: Stop/Exit 
 Main Thread: Shutdown Signal 
 Audio Engine: Stop Flag Set 
 Background Threads: Auto-terminate (daemon) 
 Main Thread: Exit 

```

## Key System Properties

### Concurrency Model
- **Type**: Multi-threaded with shared state
- **Synchronization**: Threading locks
- **Thread Count**: 4 (1 main + 3 background)
- **Thread Safety**: All shared state protected

### Real-Time Characteristics
- **Audio Latency**: 15-25ms end-to-end
- **Audio Processing**: <1ms per block
- **GUI Updates**: 100ms intervals
- **GPIO Polling**: 5ms intervals

### Communication Protocols
- **USB MIDI**: Standard MIDI protocol
- **Serial**: Custom text-based protocol
- **Internal**: Function calls (Python)

### Resource Management
- **Memory**: Shared state (minimal overhead)
- **CPU**: Multi-core utilization (Pico)
- **I/O**: Non-blocking where possible
- **Threads**: Daemon threads for cleanup

## System Boundaries

```

 SYSTEM BOUNDARY 

 Inside System: 
 Hardware: Pico + GPIO 
 Firmware: C code on Pico 
 Software: Python application 
 Communication: USB + Serial 

 Outside System: 
 macOS Operating System 
 USB Drivers (System Level) 
 Audio System (CoreAudio) 
 User Interface (Menu Bar) 

```

## Component Responsibilities

| Component | Responsibility | Dependencies |
|-----------|--------------|--------------|
| **GPIO Driver** | Key state detection | Hardware GPIO |
| **Main Firmware** | USB communication | TinyUSB, GPIO Driver |
| **USB Descriptors** | USB configuration | TinyUSB |
| **Audio Engine** | Real-time synthesis | NumPy, sounddevice |
| **MIDI Listener** | USB MIDI input | mido, Audio Engine |
| **Serial Bridge** | Serial communication | serial, Audio Engine |
| **Menu Bar GUI** | User interface | rumps, Audio Engine |
| **Orchestrator** | System coordination | All components |

## System State Machine

```
 [INITIAL STATE]

 System Startup 

 Initialization 
 Audio Engine 
 Listeners 
 GUI 

 Running State 
 Input Active 
 Audio Active 
 GUI Active 

 Error State Shutdown State
 (Recovery) (Cleanup) 

 [Running State] (reconnect)
```

---

## Summary

This system map provides a complete visual representation of:
- All system components and their relationships
- Data flow paths (2 input, 2 output)
- Thread execution model
- Communication protocols
- System boundaries and responsibilities
- State management and transitions

The architecture demonstrates a **well-structured embedded-to-desktop system** with clear separation of concerns, proper thread management, and real-time audio capabilities.

