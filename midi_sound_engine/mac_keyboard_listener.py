from enum import Enum
from pynput import keyboard
from engine import play_note, stop_note
import threading
import time

class Note(Enum):
    C = 64
    D = 66
    E = 68
    F = 69
    G = 71
    A = 72
    B = 74
    C_HIGH = 76
    D_HIGH = 77
    E_HIGH = 79
    REST = 0

# Extended Mario Theme Notes (more than 2x longer)
MARIO_NOTES = [
    Note.E_HIGH, Note.E_HIGH, Note.REST, Note.E_HIGH, Note.REST, Note.C_HIGH, Note.E_HIGH, Note.REST, Note.G_HIGH, Note.REST,
    # ... rest of the notes ...
]

note_duration = 0.2
lock = threading.Lock()

def play_mario_note(index):
    if index < 0 or index >= len(MARIO_NOTES):
        return
    note = MARIO_NOTES[index]
    if note == Note.REST:
        return  # rest
    play_note(note.value)
    time.sleep(note_duration)
    stop_note(note.value)

class MarioPlayer:
    def __init__(self):
        self.current_index = 0

    def on_press(self, key):
        try:
            char = key.char.lower()
            if char.isalpha():  # Only react to A–Z
                with lock:
                    threading.Thread(target=play_mario_note, args=(self.current_index,)).start()
                    self.current_index = (self.current_index + 1) % len(MARIO_NOTES)
        except AttributeError:
            pass

def start_keyboard_listener():
    player = MarioPlayer()
    print("⌨️ Type any letters (A–Z) to advance through the Mario melody...")
    with keyboard.Listener(on_press=player.on_press) as listener:
        listener.join()