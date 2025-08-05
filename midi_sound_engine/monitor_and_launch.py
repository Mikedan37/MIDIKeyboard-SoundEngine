# monitor_and_launch.py

from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    try:
        logging.info("Starting audio engine (main thread)...")
        start_audio_engine()

        logging.info("Launching background listeners...")
        launch_listeners()

        logging.info("Launching menu bar...")
        SynthMenuBarApp().run()

    except KeyboardInterrupt:
        # Handling keyboard interrupt and shutting down
        shutdown()
        logging.info("Synth system shut down.")
    except Exception as e:
        logging.error(f"Unexpected error: {e}")
        shutdown()

# This is a good use of the if __name__ == "__main__": idiom.
# It allows the script to be run directly or imported as a module.
if __name__ == "__main__":
    logging.basicConfig(filename='app.log', filemode='w', format='%(name)s - %(levelname)s - %(message)s')
    main()
