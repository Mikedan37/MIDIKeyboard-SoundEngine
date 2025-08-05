# monitor_and_launch.py

# Importing necessary modules
from unified_listener import launch_listeners  # Single call to launch all listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine

def main():
    """Main function to start audio engine, launch listeners and menu bar."""
    try:
        # Starting audio engine
        print("🔊 Starting audio engine (main thread)...")
        start_audio_engine()  # This comment is clear and helpful.

        # Launching background listeners
        print("🔌 Launching background listeners...")
        launch_listeners()  # This comment is clear and helpful.

        # Launching menu bar
        print("🚀 Launching menu bar...")
        SynthMenuBarApp().run()  # Running the SynthMenuBarApp

    except KeyboardInterrupt:
        # Handling keyboard interrupt and shutting down
        shutdown()
        print("🛑 Synth system shut down.")

# This is a good use of the if __name__ == "__main__": idiom.
# It allows the script to be run directly or imported as a module.
if __name__ == "__main__":
    main()
