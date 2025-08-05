# engine.py

import numpy as np
import sounddevice as sd
import threading
import time
from collections import defaultdict

SAMPLE_RATE = 44100
BLOCK_SIZE = 512
TIMEOUT = 0.3  # seconds to hold before auto stop

# 🎛️ Engine state
lock = threading.RLock()
playing = threading.Event()
held_notes = set()
note_timestamps = defaultdict(float)
phase_dict = defaultdict(float)

# 🎹 Display tracking
last_note = None
last_freq = None

def freq_from_midi(note):
    return 440.0 * (2 ** ((note - 69) / 12))

def play_note(note, velocity=100):
    global last_note, last_freq
    with lock:
        held_notes.add(note)
        note_timestamps[note] = time.time()
        phase_dict[note] = 0.0
        last_note = note
        last_freq = freq_from_midi(note)
        print(f"[ENGINE] ▶️ Playing {note} ({last_freq:.2f} Hz)")

def stop_note(note):
    with lock:
        if note in held_notes:
            held_notes.remove(note)
            print(f"[ENGINE] ⏹️ Stopped {note}")

def get_last_note_info():
    with lock:
        return last_note, last_freq

def audio_callback(outdata, frames, time_info, status):
    if status:
        print("⚠️ Audio callback warning:", status)

    buffer = np.zeros(frames, dtype=np.float32)
    now = time.time()

    with lock:
        for note in list(held_notes):
            if now - note_timestamps[note] > TIMEOUT:
                held_notes.remove(note)
                print(f"[ENGINE] ⏹️ Auto-stop {note}")
                continue

            freq = freq_from_midi(note)
            phase = phase_dict[note]
            t = np.arange(frames)
            wave = np.sin(2 * np.pi * freq * t / SAMPLE_RATE + phase)
            phase += 2 * np.pi * freq * frames / SAMPLE_RATE
            phase_dict[note] = phase % (2 * np.pi)
            buffer += wave

        buffer = np.clip(buffer, -1, 1)

    outdata[:] = buffer.reshape(-1, 1)

def configure_device():
    try:
        devices = sd.query_devices()
        for i, dev in enumerate(devices):
            if "MacBook Pro" in dev['name'] and dev['max_output_channels'] > 0:
                print(f"🔈 Using output device: {dev['name']} (index {i})")
                sd.default.device = (None, i)
                return
        print("⚠️ MacBook Pro speaker not found. Using default output.")
    except Exception as e:
        print(f"⚠️ Could not set output device: {e}")

def start_audio_engine():
    if playing.is_set():
        print("🎧 Engine already running.")
        return
    playing.set()

    print("🔊 Starting audio engine (main thread)...")
    configure_device()

    def _audio_loop():
        with sd.OutputStream(
            samplerate=SAMPLE_RATE,
            blocksize=BLOCK_SIZE,
            channels=1,
            dtype='float32',
            callback=audio_callback
        ):
            try:
                while playing.is_set():
                    sd.sleep(100)
            finally:
                playing.clear()

    threading.Thread(target=_audio_loop, daemon=True).start()

def shutdown():
    playing.clear()
    print("🛑 Audio engine shutdown requested.")