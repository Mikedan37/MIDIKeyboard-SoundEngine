# TODO: Add input debounce logic
def handle_key_press(key_event):
    if key_event.pressed:
        # Validate input
        if validate_note(key_event.note):
            # Debounce logic
            if not is_key_still_pressed(key_event.note):
                process_note_on(key_event.note)
    else:
        if validate_note(key_event.note):
            process_note_off(key_event.note)

def process_note_on(note):
    print(f"Note ON: {note}")

def process_note_off(note):
    print(f"Note OFF: {note}")

# Something New  No changes
# TODO: Fix this function
def broken_code():
    print("oops")  # Fixed syntax error

# Validate note input
def validate_note(note):
    # Assuming notes are integers in the range 0-127 (MIDI standard)
    return 0 <= note <= 127

# Check if key is still pressed
def is_key_still_pressed(note):
    # Implement logic to check if the key corresponding to the note is still pressed
    pass