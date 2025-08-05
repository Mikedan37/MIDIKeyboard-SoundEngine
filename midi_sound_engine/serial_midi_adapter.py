from pynput import keyboard
from engine import play_note, stop_note
import threading
import time

MARIO_NOTES = [
    # ... same as before ...
]

class MarioPlayer:
    def __init__(self):
        self.note_duration = 0.2
        self.current_index = 0
        self.lock = threading.Lock()

    def play_mario_note(self, index):
        note = MARIO_NOTES[index]
        if note == 0:
            return  # rest
        play_note(note)
        time.sleep(self.note_duration)
        stop_note(note)

    def on_press(self, key):
        try:
            char = key.char.lower()
            if char.isalpha():  # Only react to A–Z
                with self.lock:
                    threading.Thread(target=self.play_mario_note, args=(self.current_index,)).start()
                    self.current_index = (self.current_index + 1) % len(MARIO_NOTES)
        except AttributeError:
            pass
        except Exception as e:  # Catch all other exceptions
            print(f"Unexpected error: {e}")

    def start_keyboard_listener(self):
        print("⌨️ Type any letters (A–Z) to advance through the Mario melody...")
        with keyboard.Listener(on_press=self.on_press) as listener:
            listener.join()

# Usage
player = MarioPlayer()
player.start_keyboard_listener()