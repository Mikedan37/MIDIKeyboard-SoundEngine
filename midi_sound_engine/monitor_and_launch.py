# monitor_and_launch.py

from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine
import logging

def configure_logging():
    """Configure logging for the application"""
    logging.basicConfig(filename='app.log', filemode='w', format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

def start_engine():
    """Start the audio engine"""
    logging.info("Starting audio engine (main thread)...")
    start_audio_engine()

def launch_background_listeners():
    """Launch background listeners"""
    logging.info("Launching background listeners...")
    launch_listeners()

def launch_menu_bar():
    """Launch the menu bar"""
    logging.info("Launching menu bar...")
    SynthMenuBarApp().run()

def handle_keyboard_interrupt():
    """Handle keyboard interrupt and shut down"""
    shutdown()
    logging.info("Synth system shut down.")

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    try:
        configure_logging()
        start_engine()
        launch_background_listeners()
        launch_menu_bar()

    except KeyboardInterrupt:
        handle_keyboard_interrupt()
    except Exception as e:
        logging.error(f"Unexpected error: {e}")
        shutdown()

if __name__ == "__main__":
    main()