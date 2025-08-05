# monitor_and_launch.py

# Importing necessary modules
from unified_listener import launch_listeners  # Single call to launch all listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging

logging.basicConfig(level=logging.INFO)

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    try:
        # Starting audio engine
        logging.info("🔊 Starting audio engine (main thread)...")
        start_audio_engine()  

        # Launching background listeners
        logging.info("🔌 Launching background listeners...")
        launch_listeners()  

        # Launching menu bar
        logging.info("🚀 Launching menu bar...")
        SynthMenuBarApp().run()  

    except KeyboardInterrupt:
        # Handling keyboard interrupt and shutting down
        shutdown()
        logging.info("🛑 Synth system shut down.")
    except Exception as e:
        logging.error(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    main()