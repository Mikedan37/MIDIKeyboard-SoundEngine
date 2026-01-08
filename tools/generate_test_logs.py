#!/usr/bin/env python3
"""
Generate realistic test logs for instrumentation testing without hardware.

Simulates firmware and Python instrumentation output to test the analysis tool.
"""

import time
import random
import sys

def generate_test_logs(output_file, duration_seconds=10, key_presses=20):
    """Generate realistic instrumentation logs."""
    
    start_time_us = 1000000  # 1 second after boot
    start_time_ns = int(time.time_ns())
    
    scan_period_base = 1850  # Base scan period in microseconds
    scan_counter = 0
    
    with open(output_file, 'w') as f:
        # Generate scan periods (every 512 scans)
        scans_per_log = 512
        total_scans = int((duration_seconds * 500))  # 500 Hz scan rate
        
        for i in range(0, total_scans, scans_per_log):
            # Add some variation to scan period
            scan_period = scan_period_base + random.randint(-50, 50)
            f.write(f"SCAN_PERIOD_US,{scan_period}\n")
            scan_counter += scans_per_log
        
        # Generate key press events
        for press_num in range(key_presses):
            # Random key (0-23)
            key = random.randint(0, 23)
            midi_note = 60 + key  # C4 to B5
            
            # Random velocity timing (dt_us)
            # Soft: > 30000, Medium: 10000-30000, Hard: < 10000
            press_type = random.choice(['soft', 'medium', 'hard'])
            if press_type == 'soft':
                dt_us = random.randint(30000, 50000)
                expected_vel = random.randint(1, 45)
            elif press_type == 'medium':
                dt_us = random.randint(10000, 30000)
                expected_vel = random.randint(45, 100)
            else:  # hard
                dt_us = random.randint(100, 10000)
                expected_vel = random.randint(100, 127)
            
            # Calculate velocity from dt (simplified formula)
            if dt_us < 100:
                vel = 127
            elif dt_us > 50000:
                vel = 1
            else:
                vel = 127 - int((dt_us - 100) * 126 / 49900)
                vel = max(1, min(127, vel))
            
            # Use expected velocity for more realistic distribution
            vel = expected_vel
            
            # Timestamps
            t_event_us = start_time_us + (press_num * 50000) + random.randint(0, 10000)
            t_send_us = t_event_us + random.randint(30, 100)  # Firmware latency
            
            # Host timestamps (nanoseconds, absolute)
            t_receive_ns = start_time_ns + (press_num * 50000000) + random.randint(1000, 5000)
            t_audio_ns = t_receive_ns + random.randint(500, 2000)  # Audio pipeline latency
            
            # Write log entries
            f.write(f"VEL_SAMPLE,key={key},dt_us={dt_us},vel={vel}\n")
            f.write(f"NOTE_EVENT,note={midi_note},vel={vel},t_us={t_event_us}\n")
            f.write(f"NOTE_SEND,note={midi_note},vel={vel},t_us={t_send_us}\n")
            f.write(f"MIDI_EVENT,note={midi_note},vel={vel},t_ns={t_receive_ns}\n")
            f.write(f"AUDIO_CALLBACK,t_ns={t_audio_ns}\n")
            
            # Add some scan periods between key presses
            for _ in range(5):
                scan_period = scan_period_base + random.randint(-30, 30)
                f.write(f"SCAN_PERIOD_US,{scan_period}\n")
    
    print(f"Generated test log: {output_file}")
    print(f"  Duration: ~{duration_seconds} seconds")
    print(f"  Key presses: {key_presses}")
    print(f"  Scan periods: {scan_counter // scans_per_log} samples")

if __name__ == '__main__':
    output_file = sys.argv[1] if len(sys.argv) > 1 else 'logs/test_firmware_log.txt'
    duration = int(sys.argv[2]) if len(sys.argv) > 2 else 10
    presses = int(sys.argv[3]) if len(sys.argv) > 3 else 20
    
    generate_test_logs(output_file, duration, presses)

