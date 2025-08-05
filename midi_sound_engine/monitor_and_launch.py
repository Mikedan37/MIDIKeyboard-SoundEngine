# monitor_and_launch.py

# It's good practice to import only the specific functions you need from a module.
# This can make your code more readable and efficient.
from unified_listener import launch_listeners
from synth_menu import SynthMenuBarApp
from engine import shutdown, start_audio_engine

def main():
    try:
        print("🔊 Starting audio engine (main thread)...")
        start_audio_engine()  # This comment is clear and helpful.

        print("🔌 Launching background listeners...")
        launch_listeners()  # This comment is clear and helpful.

        print("🚀 Launching menu bar...")
        # It's good practice to separate object creation from method calls.
        # This can make your code more readable and easier to debug.
        app = SynthMenuBarApp()
        app.run()
    except KeyboardInterrupt:
        shutdown()
        print("🛑 Synth system shut down.")

# This is a good use of the if __name__ == "__main__": idiom.
# It allows the script to be run directly or imported as a module.
if __name__ == "__main__":
    main()