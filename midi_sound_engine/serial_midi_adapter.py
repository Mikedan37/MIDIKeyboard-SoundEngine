import serial
import serial.tools.list_ports
from engine import play_note, stop_note
from threading import Thread
from queue import Queue

# Cache the serial port
serial_port = None

def find_serial_port():
    global serial_port
    if serial_port is not None:
        return serial_port

    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "usbmodem" in port.device:
            print(f"[SERIAL] ✅ Found: {port.device}")
            serial_port = port.device
            return serial_port
    raise IOError("[SERIAL] ❌ Pico not found!")

def read_from_serial(ser, queue):
    while True:
        line = ser.readline().decode("utf-8", errors="ignore").strip()
        queue.put(line)

def serial_to_midi_bridge():
    port = find_serial_port()
    try:
        with serial.Serial(port, 115200, timeout=1) as ser:
            print("[SERIAL] 📡 Listening to Pico Serial MIDI...")

            queue = Queue()
            read_thread = Thread(target=read_from_serial, args=(ser, queue))
            read_thread.start()

            while True:
                if not queue.empty():
                    line = queue.get()

                    print(f"[SERIAL] 📥 {line}")

                    if ':' not in line:
                        continue  # Not valid format

                    action, value = line.split(':', 1)
                    try:
                        note = int(value.strip())
                    except ValueError:
                        print(f"[WARN] Invalid note number: {value}")
                        continue

                    if action == "ON":
                        print(f"[DEBUG] ▶️  play_note({note})")
                        play_note(note)
                    elif action == "OFF":
                        print(f"[DEBUG] ⏹  stop_note({note})")
                        stop_note(note)

    except Exception as e:
        print(f"[SERIAL] ❌ Could not open port: {e}")