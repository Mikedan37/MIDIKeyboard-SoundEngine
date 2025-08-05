# monitor_and_launch.py

# Importing necessary modules
import logging

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    try:
        # Lazy import modules
        from unified_listener import launch_listeners
        from synth_menu import SynthMenuBarApp
        from engine import shutdown, start_audio_engine

        # Starting audio engine
        logging.info("🔊 Starting audio engine (main thread)...")
        start_audio_engine()

        # Launching background listeners
        logging.info("🔌 Launching background listeners...")
        launch_listeners()

        # Launching menu bar
        logging.info("🚀 Launching menu bar...")
        SynthMenuBarApp().run()

    except Exception as e:
        # Handling all exceptions and shutting down
        logging.error(f"An error occurred: {e}")
        shutdown()
        logging.info("🛑 Synth system shut down.")

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    main()