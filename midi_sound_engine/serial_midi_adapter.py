# Importing necessary modules
from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging  # Importing logging module

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    try:
        logging.info("Starting audio engine (main thread)...")  # Using logging instead of print
        start_audio_engine()

        logging.info("Launching background listeners...")
        launch_listeners()

        logging.info("Launching menu bar...")
        SynthMenuBarApp().run()

    except KeyboardInterrupt:
        shutdown()
        logging.info("Synth system shut down.")
    except Exception as e:  # Generic exception handler
        logging.error(f"An error occurred: {e}")

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)  # Setting up basic logging configuration
    main()