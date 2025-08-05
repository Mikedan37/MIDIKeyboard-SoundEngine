from pynput import keyboard
from engine import play_note, stop_note
from concurrent.futures import ThreadPoolExecutor
import time
import queue

# Extended Mario Theme Notes (more than 2x longer)
MARIO_NOTES = [
    # ... same as before ...
]

note_duration = 0.2
current_index = queue.Queue()
current_index.put(0)

def play_mario_note(index):
    # ... same as before ...

def on_press(key):
    try:
        char = key.char.lower()
        if char.isalpha():  # Only react to A–Z
            index = current_index.get()
            executor.submit(play_mario_note, index)
            current_index.put((index + 1) % len(MARIO_NOTES))
    except AttributeError:
        pass

def start_keyboard_listener():
    print("⌨️ Type any letters (A–Z) to advance through the Mario melody...")
    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()

executor = ThreadPoolExecutor(max_workers=10)