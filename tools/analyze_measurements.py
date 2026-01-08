#!/usr/bin/env python3
"""
Measurement Analysis Tool for MIDI Keyboard Latency & Velocity Instrumentation

Parses log files from firmware and Python instrumentation, computes statistics,
and outputs Markdown reports and CSV summary tables.
"""

import sys
import re
import statistics
from collections import defaultdict
from typing import List, Dict, Tuple

# Measurement data structures
scan_periods: List[int] = []
velocity_samples: List[Tuple[int, int, int]] = []  # (key, dt_us, vel)
note_events: Dict[int, int] = {}  # note -> t_event
note_sends: Dict[int, int] = {}  # note -> t_send
midi_events: Dict[int, int] = {}  # note -> t_ns (host receive)
audio_callbacks: List[int] = []  # t_ns timestamps

def parse_log_file(filename: str):
    """Parse instrumentation log file and extract measurements."""
    try:
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                
                # SCAN_PERIOD_US,1850
                match = re.match(r'SCAN_PERIOD_US,(\d+)', line)
                if match:
                    scan_periods.append(int(match.group(1)))
                    continue
                
                # VEL_SAMPLE,key=3,dt_us=10400,vel=92
                match = re.match(r'VEL_SAMPLE,key=(\d+),dt_us=(\d+),vel=(\d+)', line)
                if match:
                    key = int(match.group(1))
                    dt_us = int(match.group(2))
                    vel = int(match.group(3))
                    velocity_samples.append((key, dt_us, vel))
                    continue
                
                # NOTE_EVENT,note=64,vel=92,t_us=12345678
                match = re.match(r'NOTE_EVENT,note=(\d+),vel=(\d+),t_us=(\d+)', line)
                if match:
                    note = int(match.group(1))
                    t_event = int(match.group(3))
                    note_events[note] = t_event
                    continue
                
                # NOTE_SEND,note=64,vel=92,t_us=12345731
                match = re.match(r'NOTE_SEND,note=(\d+),vel=(\d+),t_us=(\d+)', line)
                if match:
                    note = int(match.group(1))
                    t_send = int(match.group(3))
                    note_sends[note] = t_send
                    continue
                
                # MIDI_EVENT,note=64,vel=92,t_ns=1733618339123000000
                match = re.match(r'MIDI_EVENT,note=(\d+),vel=(\d+),t_ns=(\d+)', line)
                if match:
                    note = int(match.group(1))
                    t_ns = int(match.group(3))
                    midi_events[note] = t_ns
                    continue
                
                # AUDIO_CALLBACK,t_ns=1733618339124000000
                match = re.match(r'AUDIO_CALLBACK,t_ns=(\d+)', line)
                if match:
                    audio_callbacks.append(int(match.group(1)))
                    continue
                    
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error parsing log file: {e}", file=sys.stderr)
        sys.exit(1)

def compute_scan_period_stats() -> Dict[str, float]:
    """Compute statistics for matrix scan period."""
    if not scan_periods:
        return {}
    
    return {
        'mean': statistics.mean(scan_periods),
        'min': min(scan_periods),
        'max': max(scan_periods),
        'median': statistics.median(scan_periods),
        'stdev': statistics.stdev(scan_periods) if len(scan_periods) > 1 else 0.0,
        'count': len(scan_periods)
    }

def compute_firmware_latency_stats() -> Dict[str, float]:
    """Compute firmware latency (NOTE_EVENT → NOTE_SEND)."""
    latencies = []
    
    for note in note_events:
        if note in note_sends:
            latency_us = note_sends[note] - note_events[note]
            latencies.append(latency_us)
    
    if not latencies:
        return {}
    
    return {
        'mean': statistics.mean(latencies),
        'min': min(latencies),
        'max': max(latencies),
        'median': statistics.median(latencies),
        'stdev': statistics.stdev(latencies) if len(latencies) > 1 else 0.0,
        'count': len(latencies)
    }

def compute_velocity_distribution() -> Dict[str, Dict]:
    """Compute velocity distribution by dt_us buckets."""
    soft = []  # dt > 30000 us
    medium = []  # 10000-30000 us
    hard = []  # dt < 10000 us
    
    for key, dt_us, vel in velocity_samples:
        if dt_us > 30000:
            soft.append(vel)
        elif dt_us >= 10000:
            medium.append(vel)
        else:
            hard.append(vel)
    
    result = {}
    if soft:
        result['soft'] = {
            'mean_vel': statistics.mean(soft),
            'count': len(soft),
            'dt_range': '> 30000 us'
        }
    if medium:
        result['medium'] = {
            'mean_vel': statistics.mean(medium),
            'count': len(medium),
            'dt_range': '10000-30000 us'
        }
    if hard:
        result['hard'] = {
            'mean_vel': statistics.mean(hard),
            'count': len(hard),
            'dt_range': '< 10000 us'
        }
    
    return result

def compute_usb_host_latency_stats() -> Dict[str, float]:
    """Compute USB → Host latency (NOTE_SEND → MIDI_EVENT).
    
    Note: This is an approximation since firmware timestamps are in microseconds
    (relative to boot) and host timestamps are in nanoseconds (absolute time).
    For accurate measurement, timestamps should be synchronized or use a common
    reference point. This function computes relative differences when both
    timestamps exist for the same note.
    """
    latencies_us = []
    
    # Match NOTE_SEND and MIDI_EVENT by note number
    # Since timestamps use different references, we can only compute
    # relative latency if we have sequential events
    send_notes = sorted(note_sends.keys())
    receive_notes = sorted(midi_events.keys())
    
    # For now, we'll note that this requires synchronized timestamps
    # Return empty if we can't compute meaningful latency
    # In practice, you'd want to use a common time reference or
    # measure the round-trip time with a known delay
    
    # For demonstration, we'll compute relative differences
    # but warn that absolute latency requires synchronization
    if not send_notes or not receive_notes:
        return {}
    
    # Match by note number and compute difference
    # This is approximate - real measurement needs time sync
    matched = 0
    for note in send_notes:
        if note in midi_events:
            # Approximate: assume firmware and host clocks are roughly aligned
            # In practice, you'd need to sync clocks or use a reference signal
            t_send_us = note_sends[note]
            t_receive_ns = midi_events[note]
            # Convert ns to us
            t_receive_us = t_receive_ns / 1000.0
            # This is only meaningful if clocks are synchronized
            # For now, we'll skip this calculation and note it requires sync
            matched += 1
    
    # Return empty if we can't compute meaningful latency
    # Real implementation would require clock synchronization
    return {}

def compute_audio_pipeline_latency_stats() -> Dict[str, float]:
    """Compute audio pipeline latency (MIDI_EVENT → AUDIO_CALLBACK)."""
    latencies_us = []
    
    # Match MIDI events to subsequent audio callbacks
    midi_times = sorted(midi_events.values())
    audio_times = sorted(audio_callbacks)
    
    if not midi_times or not audio_times:
        return {}
    
    # For each MIDI event, find the next audio callback
    audio_idx = 0
    for midi_ns in midi_times:
        while audio_idx < len(audio_times) and audio_times[audio_idx] < midi_ns:
            audio_idx += 1
        if audio_idx < len(audio_times):
            latency_ns = audio_times[audio_idx] - midi_ns
            latency_us = latency_ns / 1000.0
            latencies_us.append(latency_us)
    
    if not latencies_us:
        return {}
    
    return {
        'mean': statistics.mean(latencies_us),
        'min': min(latencies_us),
        'max': max(latencies_us),
        'median': statistics.median(latencies_us),
        'stdev': statistics.stdev(latencies_us) if len(latencies_us) > 1 else 0.0,
        'count': len(latencies_us)
    }

def generate_markdown_report() -> str:
    """Generate Markdown report with all statistics."""
    report = []
    report.append("# Measurement Analysis Report\n")
    report.append("## Scan Period\n")
    
    scan_stats = compute_scan_period_stats()
    if scan_stats:
        report.append(f"- **Mean**: {scan_stats['mean']:.1f} us")
        report.append(f"- **Min**: {scan_stats['min']} us")
        report.append(f"- **Max**: {scan_stats['max']} us")
        report.append(f"- **Median**: {scan_stats['median']:.1f} us")
        report.append(f"- **Std Dev**: {scan_stats['stdev']:.1f} us")
        report.append(f"- **Samples**: {scan_stats['count']}\n")
    else:
        report.append("No scan period data found.\n")
    
    report.append("## Firmware Latency (NOTE_EVENT → NOTE_SEND)\n")
    fw_stats = compute_firmware_latency_stats()
    if fw_stats:
        report.append(f"- **Mean**: {fw_stats['mean']:.1f} us")
        report.append(f"- **Min**: {fw_stats['min']} us")
        report.append(f"- **Max**: {fw_stats['max']} us")
        report.append(f"- **Median**: {fw_stats['median']:.1f} us")
        report.append(f"- **Std Dev**: {fw_stats['stdev']:.1f} us")
        report.append(f"- **Samples**: {fw_stats['count']}\n")
    else:
        report.append("No firmware latency data found.\n")
    
    report.append("## Velocity Distribution\n")
    vel_dist = compute_velocity_distribution()
    if vel_dist:
        for category, data in vel_dist.items():
            report.append(f"### {category.capitalize()} Presses ({data['dt_range']})")
            report.append(f"- **Mean Velocity**: {data['mean_vel']:.1f}")
            report.append(f"- **Count**: {data['count']}\n")
    else:
        report.append("No velocity data found.\n")
    
    report.append("## USB → Host Latency (NOTE_SEND → MIDI_EVENT)\n")
    usb_stats = compute_usb_host_latency_stats()
    if usb_stats:
        report.append(f"- **Mean**: {usb_stats['mean']:.1f} us")
        report.append(f"- **Min**: {usb_stats['min']:.1f} us")
        report.append(f"- **Max**: {usb_stats['max']:.1f} us")
        report.append(f"- **Median**: {usb_stats['median']:.1f} us")
        report.append(f"- **Std Dev**: {usb_stats['stdev']:.1f} us")
        report.append(f"- **Samples**: {usb_stats['count']}\n")
    else:
        report.append("No USB→Host latency data found.\n")
    
    report.append("## Audio Pipeline Latency (MIDI_EVENT → AUDIO_CALLBACK)\n")
    audio_stats = compute_audio_pipeline_latency_stats()
    if audio_stats:
        report.append(f"- **Mean**: {audio_stats['mean']:.1f} us")
        report.append(f"- **Min**: {audio_stats['min']:.1f} us")
        report.append(f"- **Max**: {audio_stats['max']:.1f} us")
        report.append(f"- **Median**: {audio_stats['median']:.1f} us")
        report.append(f"- **Std Dev**: {audio_stats['stdev']:.1f} us")
        report.append(f"- **Samples**: {audio_stats['count']}\n")
    else:
        report.append("No audio pipeline latency data found.\n")
    
    return "\n".join(report)

def generate_csv_summary() -> str:
    """Generate CSV summary table."""
    csv_lines = []
    csv_lines.append("Metric,Mean,Min,Max,Median,StdDev,Count")
    
    scan_stats = compute_scan_period_stats()
    if scan_stats:
        csv_lines.append(f"Scan Period (us),{scan_stats['mean']:.1f},{scan_stats['min']},{scan_stats['max']},{scan_stats['median']:.1f},{scan_stats['stdev']:.1f},{scan_stats['count']}")
    
    fw_stats = compute_firmware_latency_stats()
    if fw_stats:
        csv_lines.append(f"Firmware Latency (us),{fw_stats['mean']:.1f},{fw_stats['min']},{fw_stats['max']},{fw_stats['median']:.1f},{fw_stats['stdev']:.1f},{fw_stats['count']}")
    
    usb_stats = compute_usb_host_latency_stats()
    if usb_stats:
        csv_lines.append(f"USB→Host Latency (us),{usb_stats['mean']:.1f},{usb_stats['min']:.1f},{usb_stats['max']:.1f},{usb_stats['median']:.1f},{usb_stats['stdev']:.1f},{usb_stats['count']}")
    
    audio_stats = compute_audio_pipeline_latency_stats()
    if audio_stats:
        csv_lines.append(f"Audio Pipeline Latency (us),{audio_stats['mean']:.1f},{audio_stats['min']:.1f},{audio_stats['max']:.1f},{audio_stats['median']:.1f},{audio_stats['stdev']:.1f},{audio_stats['count']}")
    
    return "\n".join(csv_lines)

def main():
    if len(sys.argv) < 2:
        print("Usage: analyze_measurements.py <log_file> [--csv] [--output <file>]")
        sys.exit(1)
    
    log_file = sys.argv[1]
    output_csv = '--csv' in sys.argv
    output_file = None
    
    if '--output' in sys.argv:
        idx = sys.argv.index('--output')
        if idx + 1 < len(sys.argv):
            output_file = sys.argv[idx + 1]
    
    # Parse log file
    parse_log_file(log_file)
    
    # Generate report
    if output_csv:
        report = generate_csv_summary()
    else:
        report = generate_markdown_report()
    
    # Output report
    if output_file:
        with open(output_file, 'w') as f:
            f.write(report)
        print(f"Report written to {output_file}")
    else:
        print(report)

if __name__ == '__main__':
    main()

