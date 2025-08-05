import logging
from sounddevice import PortAudioError

# Setup logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Use threading.Event for thread-safe start/stop control
playing = threading.Event()

def play_note(note, velocity=100):
    # Validate note
    if not isinstance(note, int) or note < 0 or note > 127:
        logger.error(f"Invalid note: {note}")
        return

    # ...

def stop_note(note):
    # Validate note
    if not isinstance(note, int) or note < 0 or note > 127:
        logger.error(f"Invalid note: {note}")
        return

    # ...

def start_audio_engine():
    if playing.is_set():
        logger.info("Engine already running.")
        return
    playing.set()

    # ...

    try:
        # ...
    except PortAudioError as e:
        logger.error(f"Could not set output device: {e}")

    # ...

def shutdown():
    playing.clear()
    logger.info("Audio engine shutdown requested.")