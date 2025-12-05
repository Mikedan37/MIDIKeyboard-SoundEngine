import time
import threading
import mido
from engine import play_note, stop_note

def midi_listener():
    print("🔌 Starting Pico MIDI listener thread...")
    while True:
        try:
            # Find the Pico (USB MIDI device)
            inputs = mido.get_input_names()
            pico_input = next((name for name in inputs if 'Pico' in name or 'MIDI' in name), None)

            if not pico_input:
                print("❌ Pico MIDI not found. Retrying in 2s...")
                time.sleep(2)
                continue

            print(f"🎹 Connected to: {pico_input}")
            with mido.open_input(pico_input) as inport:
                for msg in inport:
                    if msg.type == 'note_on' and msg.velocity > 0:
                        print(f"[PICO PLAY] Note {msg.note}")
                        play_note(msg.note, velocity=msg.velocity)
                    elif msg.type in ['note_off', 'note_on'] and msg.velocity == 0:
                        print(f"[PICO STOP] Note {msg.note}")
                        stop_note(msg.note)

        except Exception as e:
            print(f"⚠️ MIDI error: {e}. Reconnecting in 2s...")
            time.sleep(2)


            