from concurrent.futures import ThreadPoolExecutor
from pynput import keyboard
from engine import play_note, stop_note
import time

MARIO_NOTES = [
    76, 76, 0, 76, 0, 72, 76, 0, 79, 0,         
    0, 67, 0, 0, 72, 0, 67, 0, 64, 0,           
    69, 0, 71, 70, 68, 66, 68, 70, 71, 69,       
    67, 69, 71, 72, 74, 76, 77, 79,             
    76, 74, 72, 71, 72                          
]

class MarioPlayer:
    def __init__(self, note_duration=0.2):
        self.note_duration = note_duration
        self.current_index = 0
        self.executor = ThreadPoolExecutor(max_workers=10)

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
                self.executor.submit(self.play_mario_note, self.current_index)
                self.current_index = (self.current_index + 1) % len(MARIO_NOTES)
        except AttributeError:
            pass

    def start_keyboard_listener(self):
        print("⌨️ Type any letters (A–Z) to advance through the Mario melody...")
        with keyboard.Listener(on_press=self.on_press) as listener:
            listener.join()

player = MarioPlayer()
player.start_keyboard_listener()