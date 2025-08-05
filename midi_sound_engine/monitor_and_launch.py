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
        logging.info("Synth system shut down.")
        shutdown()
    except Exception as e:
        logging.error("Unexpected error: %s", e, exc_info=True)
        shutdown()

if __name__ == "__main__":
    logging.basicConfig(filename='app.log', filemode='w', level=logging.INFO, format='%(name)s - %(levelname)s - %(message)s')
    main()