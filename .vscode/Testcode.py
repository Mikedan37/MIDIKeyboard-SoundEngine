import time

# TODO: Add input debounce logic
def handle_key_press(key_event):
    # Validate key_event object
    if not hasattr(key_event, 'pressed') or not hasattr(key_event, 'note'):
        print("Invalid key event")
        return

    last_key_press_time = 0
    debounce_time = 0.3  # time in seconds

    if time.time() - last_key_press_time < debounce_time:
        return

    last_key_press_time = time.time()

    if key_event.pressed:
        # Validate note
        if not isinstance(key_event.note, str):
            print("Invalid note")
            return
        process_note_on(key_event.note)
    else:
        process_note_off(key_event.note)

def process_note_on(note):
    # Validate note
    if not isinstance(note, str):
        print("Invalid note")
        return
    print(f"Note ON: {note}")

def process_note_off(note):
    # Validate note
    if not isinstance(note, str):
        print("Invalid note")
        return
    print(f"Note OFF: {note}")

# Something New  No changes
# TODO: Fix this function
def broken_code():
    raise NotImplementedError("This function is not yet implemented")