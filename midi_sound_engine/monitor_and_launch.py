from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging


class LoggingContextManager:
    """Context manager for logging."""

    def __enter__(self):
        logging.basicConfig(
            filename="app.log",
            filemode="w",
            format="%(name)s - %(levelname)s - %(message)s",
        )
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        # Close the logger here if necessary
        pass


def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    with LoggingContextManager():
        try:
            logging.info("Starting audio engine (main thread)...")
            start_audio_engine()

            logging.info("Launching background listeners...")
            launch_listeners()

            logging.info("Launching menu bar...")
            SynthMenuBarApp().run()

        except KeyboardInterrupt:
            logging.info("Synth system shut down.")
            shutdown()  # Shut down before re-raising the exception
            raise
        except Exception as e:  # Consider catching a more specific exception
            logging.error(f"Unexpected error: {e}")
            shutdown()  # Shut down before re-raising the exception
            raise


if __name__ == "__main__":
    main()
