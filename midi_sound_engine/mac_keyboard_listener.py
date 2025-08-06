from pynput import keyboard
from engine import play_note, stop_note
from concurrent.futures import ThreadPoolExecutor
import time

# ... (rest of the code remains the same)

executor = ThreadPoolExecutor(max_workers=10)

def play_mario_note(index):
    note = MARIO_NOTES[index]
    if note == 0:
        return  # rest
    try:
        play_note(note)
        time.sleep(note_duration)
        stop_note(note)
    except Exception as e:
        print(f"Error playing note: {e}")

def on_press(key):
    global current_index
    try:
        char = key.char.lower()
        if char.isalpha():  # Only react to A–Z
            with lock:
                executor.submit(play_mario_note, current_index)
                current_index = (current_index + 1) % len(MARIO_NOTES)
    except AttributeError:
        pass

# ... (rest of the code remains the same)