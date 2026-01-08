# unified_listener.py

import threading
from pico_listener import midi_listener
from serial_midi_adapter import serial_to_midi_bridge

def launch_listeners():
    threads = [
        threading.Thread(target=midi_listener, daemon=True),
        threading.Thread(target=serial_to_midi_bridge, daemon=True),
    ]
    for t in threads:
        t.start()