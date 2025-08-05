import time


# TODO: Add input debounce logic
def handle_key_press(key_event):
    if validate_note(key_event.note):
        if key_event.pressed:
            # add debounce logic
            time.sleep(0.01)  # adjust delay as needed
            process_note_on(key_event.note)
        else:
            # add debounce logic
            time.sleep(0.01)  # adjust delay as needed
            process_note_off(key_event.note)


def validate_note(note):
    # TODO: implement note validation logic
    # return False if note is not valid
    return True


def process_note_on(note):
    print(f"Note ON: {note}")


def process_note_off(note):
    print(f"Note OFF: {note}")


# Something New  No changes
# TODO: Fix this function
def broken_code():
    print("oops")
