from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging

def configure_logging():
    """Configure logging for the application."""
    # Consider storing the log file in a secure location and/or implementing log file encryption.
    logging.basicConfig(filename='app.log', filemode='w', format='%(name)s - %(levelname)s - %(message)s')

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    configure_logging()
    try:
        logging.info("Starting audio engine (main thread)...")
        start_audio_engine()

        logging.info("Launching background listeners...")
        launch_listeners()

        logging.info("Launching menu bar...")
        SynthMenuBarApp().run()

    except KeyboardInterrupt:
        # Consider handling this signal and implementing a secure shutdown procedure.
        logging.info("Synth system shut down.")
        raise
    except Exception as e:
        # If the exception message (`e`) contains sensitive data, it will be written to the log file.
        # This could potentially be a security risk. Consider sanitizing the data that is written to the log file.
        logging.error(f"Unexpected error: {e}")
        raise
    finally:
        shutdown()

if __name__ == "__main__":
    main()