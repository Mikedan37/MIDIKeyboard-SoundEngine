import serial
import serial.tools.list_ports
from engine import play_note, stop_note
import logging  # import logging module

# Set up logging
logging.basicConfig(level=logging.DEBUG)

def find_serial_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "usbmodem" in port.device:
            logging.info(f"[SERIAL] ✅ Found: {port.device}")  # use logging instead of print
            return port.device
    raise IOError("[SERIAL] ❌ Pico not found!")

def serial_to_midi_bridge():
    port = find_serial_port()
    try:
        with serial.Serial(port, 115200, timeout=1) as ser:
            logging.info("[SERIAL] 📡 Listening to Pico Serial MIDI...")  # use logging instead of print
            while True:
                try:
                    line = ser.readline().decode("utf-8", errors="ignore").strip()
                    if not line:
                        continue

                    logging.info(f"[SERIAL] 📥 {line}")  # use logging instead of print

                    if ':' not in line:
                        continue  # Not valid format

                    action, value = line.split(':', 1)
                    try:
                        note = int(value.strip())
                    except ValueError:
                        logging.warning(f"[WARN] Invalid note number: {value}")  # use logging instead of print
                        continue

                    # Validate action and note values
                    if action not in ["ON", "OFF"] or not 0 <= note <= 127:
                        logging.warning(f"[WARN] Invalid action or note value: {action}, {note}")  # use logging instead of print
                        continue

                    if action == "ON":
                        logging.debug(f"[DEBUG] ▶️  play_note({note})")  # use logging instead of print
                        play_note(note)
                    elif action == "OFF":
                        logging.debug(f"[DEBUG] ⏹  stop_note({note})")  # use logging instead of print
                        stop_note(note)

                except serial.SerialException as e:  # catch specific exception
                    logging.error(f"[SERIAL] ⚠️ Error: {e}")  # use logging instead of print

    except serial.SerialException as e:  # catch specific exception
        logging.error(f"[SERIAL] ❌ Could not open port: {e}")  # use logging instead of print