# TODO: Add input debounce logic
def handle_key_press(key_event):
    if key_event.pressed:
        # risky: no debounce or input validation
        # Suggestion: Add input validation to ensure the key_event.note is within the expected range
        if validate_note(key_event.note):
            process_note_on(key_event.note)
        else:
            print("Invalid note")
    else:
        if validate_note(key_event.note):
            process_note_off(key_event.note)
        else:
            print("Invalid note")

def process_note_on(note):
    print(f"Note ON: {note}")

def process_note_off(note):
    print(f"Note OFF: {note}")

# Suggestion: Define a function to validate note
def validate_note(note):
    # Assuming notes should be integers from 0 to 127 (MIDI standard)
    if isinstance(note, int) and 0 <= note <= 127:
        return True
    else:
        return False

# Something New  No changes
# TODO: Fix this function
def broken_code():
    print("oops")  # Suggestion: Provide more context in the print statement or handle the error properly