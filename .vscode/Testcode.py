import time

# TODO: Add input debounce logic
def debounce(wait):
    # Decorator that will prevent a function from being called if it was called less than wait time ago.
    def decorator(fn):
        def debounced(*args, **kwargs):
            last_called = getattr(debounced, '_last_called', None)
            now = time.time()
            if last_called is None or now - last_called > wait:
                result = fn(*args, **kwargs)
                debounced._last_called = now
                return result
        return debounced
    return decorator

@debounce(0.1)
def handle_key_press(key_event):
    # Validate key_event.note before processing
    if isinstance(key_event.note, int) and 0 <= key_event.note <= 127:
        if key_event.pressed:
            process_note_on(key_event.note)
        else:
            process_note_off(key_event.note)
    else:
        print("Invalid note")

def process_note_on(note):
    print(f"Note ON: {note}")

def process_note_off(note):
    print(f"Note OFF: {note}")

# TODO: Fix this function
def broken_code():
    raise NotImplementedError("This function is not yet implemented")