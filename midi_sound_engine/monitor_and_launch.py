#!/usr/bin/env python3
"""
QWERTY MIDI Keyboard - Monitor and Auto-Launch
Monitors for Pico connection and launches synthesizer automatically.
"""

import sys
import os
import time
import subprocess
from pathlib import Path

# Add current directory to path
sys.path.insert(0, str(Path(__file__).parent))

try:
    from unified_listener import launch_listeners
    from engine import shutdown, start_audio_engine
except ImportError as e:
    print(f"Import error: {e}")
    print("Make sure you're running from the midi_sound_engine directory")
    sys.exit(1)

# macOS-only menu bar (optional)
try:
    from synth_menu import SynthMenuBarApp
    HAS_MENU_BAR = True
except ImportError:
    HAS_MENU_BAR = False


def detect_pico():
    """Detect if Pico is connected via USB."""
    import platform
    
    system = platform.system()
    
    if system == "Darwin":  # macOS
        try:
            result = subprocess.run(
                ["system_profiler", "SPUSBDataType"],
                capture_output=True,
                text=True,
                timeout=5
            )
            return "RP2040" in result.stdout or "Pico" in result.stdout
        except (subprocess.TimeoutExpired, FileNotFoundError, subprocess.SubprocessError):
            return False
    
    elif system == "Linux":
        try:
            result = subprocess.run(
                ["lsusb"],
                capture_output=True,
                text=True,
                timeout=5
            )
            return "RP2040" in result.stdout or "2e8a" in result.stdout.lower()
        except (subprocess.TimeoutExpired, FileNotFoundError, subprocess.SubprocessError):
            return False
    
    elif system == "Windows":
        try:
            # Check for Pico in device manager via PowerShell
            result = subprocess.run(
                ["powershell", "-Command", 
                 "Get-PnpDevice | Where-Object {$_.FriendlyName -like '*Pico*' -or $_.FriendlyName -like '*RP2040*'}"],
                capture_output=True,
                text=True,
                timeout=5
            )
            return result.returncode == 0 and result.stdout.strip()
        except (subprocess.TimeoutExpired, FileNotFoundError, subprocess.SubprocessError):
            return False
    
    return False


def main():
    """Main entry point - launches synthesizer."""
    print("QWERTY MIDI Keyboard - Starting...")
    print("=" * 50)
    
    # Check if Pico is connected
    if not detect_pico():
        print("Pico not detected. Continuing anyway...")
        print("   (Plug in Pico to use MIDI input)")
    else:
        print("Pico detected!")
    
    print("")
    
    try:
        print("Starting audio engine...")
        start_audio_engine()
        
        print("Launching background listeners...")
        launch_listeners()
        
        print("")
        print("Synthesizer running!")
        print("Press Ctrl+C to stop")
        print("")
        
        # Run menu bar app on macOS, otherwise just wait
        if HAS_MENU_BAR:
            print("Launching menu bar interface (macOS)...")
            SynthMenuBarApp().run()
        else:
            # On Windows/Linux, just keep running
            print("Running in background mode (no GUI)")
            print("Use Ctrl+C to stop")
            try:
                while True:
                    time.sleep(1)
            except KeyboardInterrupt:
                pass
        
    except KeyboardInterrupt:
        print("\nShutting down...")
        shutdown()
        print("Shutdown complete.")
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        shutdown()
        sys.exit(1)


if __name__ == "__main__":
    main()
