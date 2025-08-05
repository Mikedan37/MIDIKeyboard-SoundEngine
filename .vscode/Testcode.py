import time

# TODO: Add input debounce logic
def handle_key_press(key_event):
    # Add debounce logic
    debounce_time = 0.1  # Adjust this value as needed
    time.sleep(debounce_time)
    
    # Add input validation
    if not isinstance(key_event.note, str):
        raise ValueError("Note should be a string")
    
    if key_event.pressed:
        process_note_on(key_event.note)
    else:
        process_note_off(key_event.note)

def process_note_on(note):
    print(f"Note ON: {note}")

def process_note_off(note):
    print(f"Note OFF: {note}")

# TODO: Fix this function
def broken_code():
    # Fixed syntax error
    print("oops")