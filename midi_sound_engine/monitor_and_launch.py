from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging
import traceback


def configure_logging():
    """Configure logging for the application."""
    logging.basicConfig(
        filename="app.log",
        filemode="w",
        format="%(name)s - %(levelname)s - %(message)s",
        level=logging.INFO,
    )

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
        logging.info("Synth system shut down.")
        raise
    except Exception as e:
        logging.error(f"Unexpected error: {e}\n{traceback.format_exc()}")
        raise
    finally:
        shutdown()


if __name__ == "__main__":
    main()
