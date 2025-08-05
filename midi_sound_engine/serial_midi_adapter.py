# TODO: Add input debounce logic
def handle_key_press(key_event):
    if key_event.pressed:
        # risky: no debounce or input validation
        # Suggestion: Validate the key_event.note before processing it.
        # This can prevent potential security issues like code injection or buffer overflow.
        if validate_note(key_event.note):
            process_note_on(key_event.note)
        else:
            print("Invalid note")
    else:
        process_note_off(key_event.note)

def process_note_on(note):
    print(f"Note ON: {note}")

def process_note_off(note):
    print(f"Note OFF: {note}")

# Something New  No changes
# TODO: Fix this function
def broken_code():
    # Suggestion: Provide more information about what's broken in this function.
    # This can help other developers understand what needs to be fixed.
    print("oops")  # What's the error here?