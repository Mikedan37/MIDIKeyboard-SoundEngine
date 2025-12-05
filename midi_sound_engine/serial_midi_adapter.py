import serial
import serial.tools.list_ports
from engine import play_note, stop_note

def find_serial_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "usbmodem" in port.device:
            print(f"[SERIAL] ✅ Found: {port.device}")
            return port.device
    raise IOError("[SERIAL] ❌ Pico not found!")

def serial_to_midi_bridge():
    port = find_serial_port()
    try:
        with serial.Serial(port, 115200, timeout=1) as ser:
            print("[SERIAL] 📡 Listening to Pico Serial MIDI...")
            while True:
                try:
                    line = ser.readline().decode("utf-8", errors="ignore").strip()
                    if not line:
                        continue

                    print(f"[SERIAL] 📥 {line}")

                    if ':' not in line:
                        continue  # Not valid format

                    action, value = line.split(':', 1)
                    try:
                        note = int(value.strip())
                        # Validate note number range
                        if note < 0 or note > 127:
                            raise ValueError("Note number out of range")
                    except ValueError as e:
                        print(f"[WARN] Invalid note number: {value}. Reason: {e}")
                        continue

                    if action == "ON":
                        print(f"[DEBUG] ▶️  play_note({note})")
                        play_note(note)
                    elif action == "OFF":
                        print(f"[DEBUG] ⏹  stop_note({note})")
                        stop_note(note)

                # Catch and log generic error message
                except Exception:
                    print("[SERIAL] ⚠️ Error occurred during processing")

    # Catch and log generic error message
    except Exception:
        print("[SERIAL] ❌ Could not open port")