import time
import logging
import serial
import serial.tools.list_ports
from engine import play_note, stop_note

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def find_serial_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "usbmodem" in port.device:
            logger.info(f"[SERIAL] ✅ Found: {port.device}")
            return port.device
    raise IOError("[SERIAL] ❌ Pico not found!")

def process_line(line):
    if ':' not in line:
        return  # Not valid format

    action, value = line.split(':', 1)
    try:
        note = int(value.strip())
    except ValueError:
        logger.warning(f"Invalid note number: {value}")
        return

    if action == "ON":
        logger.debug(f"▶️  play_note({note})")
        play_note(note)
    elif action == "OFF":
        logger.debug(f"⏹  stop_note({note})")
        stop_note(note)

def serial_to_midi_bridge():
    port = find_serial_port()
    try:
        with serial.Serial(port, 115200, timeout=1) as ser:
            logger.info("[SERIAL] 📡 Listening to Pico Serial MIDI...")
            while True:
                try:
                    line = ser.readline().decode("utf-8", errors="ignore").strip()
                    if line:
                        logger.debug(f"[SERIAL] 📥 {line}")
                        process_line(line)
                except Exception as e:
                    logger.error(f"[SERIAL] ⚠️ Error: {e}")
                time.sleep(0.01)  # Add a small delay to reduce CPU usage
    except Exception as e:
        logger.error(f"[SERIAL] ❌ Could not open port: {e}")