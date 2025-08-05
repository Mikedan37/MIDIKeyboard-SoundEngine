import numpy as np
import sounddevice as sd
import threading
import time

class AudioEngine:
    def __init__(self, sample_rate=44100, block_size=512, timeout=0.3):
        self.sample_rate = sample_rate
        self.block_size = block_size
        self.timeout = timeout

        self.lock = threading.Lock()
        self.playing = False
        self.held_notes = set()
        self.note_timestamps = {}
        self.phase_dict = {}

        self.last_note = None
        self.last_freq = None

    def freq_from_midi(self, note):
        return 440.0 * (2 ** ((note - 69) / 12))

    def play_note(self, note, velocity=100):
        with self.lock:
            self.held_notes.add(note)
            self.note_timestamps[note] = time.time()
            if note not in self.phase_dict:
                self.phase_dict[note] = 0.0
            self.last_note = note
            self.last_freq = self.freq_from_midi(note)
            print(f"[ENGINE] ▶️ Playing {note} ({self.last_freq:.2f} Hz)")

    # ... rest of the methods ...