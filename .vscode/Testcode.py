# TODO: Add input debounce logic
def handle_key_press(key_event):
    if key_event.pressed:
        # risky: no debounce or input validation
        process_note_on(key_event.note)
    else:
        process_note_off(key_event.note)

def process_note_on(note):
    print(f"Note ON: {note}")

def process_note_off(note):
    print(f"Note OFF: {note}")

# Something New  No changes
# TODO: Fix this function
def broken_code():
    print("oops")