# Measurement Tools

This directory contains tools for instrumenting and analyzing the MIDI keyboard's performance metrics.

## analyze_measurements.py

Parses instrumentation logs and generates statistical analysis reports.

### Usage

```bash
# Generate Markdown report
python3 tools/analyze_measurements.py logs/firmware_log.txt

# Generate CSV summary
python3 tools/analyze_measurements.py logs/firmware_log.txt --csv

# Save to file
python3 tools/analyze_measurements.py logs/firmware_log.txt --output logs/report.md
```

### Output Format

The script parses the following log formats:

- `SCAN_PERIOD_US,<microseconds>` - Matrix scan period
- `VEL_SAMPLE,key=<n>,dt_us=<us>,vel=<1-127>` - Velocity timing measurement
- `NOTE_EVENT,note=<n>,vel=<v>,t_us=<us>` - Firmware event detection timestamp
- `NOTE_SEND,note=<n>,vel=<v>,t_us=<us>` - USB send timestamp
- `MIDI_EVENT,note=<n>,vel=<v>,t_ns=<nanoseconds>` - Host receive timestamp
- `AUDIO_CALLBACK,t_ns=<nanoseconds>` - Audio callback timestamp

### Computed Metrics

1. **Scan Period Statistics**: Mean, min, max, median, std dev of matrix scan times
2. **Firmware Latency**: Time from event detection to USB send (NOTE_EVENT → NOTE_SEND)
3. **Velocity Distribution**: Categorized by press speed (soft/medium/hard)
4. **USB→Host Latency**: Time from USB send to host receive (NOTE_SEND → MIDI_EVENT)
5. **Audio Pipeline Latency**: Time from MIDI receive to audio callback (MIDI_EVENT → AUDIO_CALLBACK)

## Measurement Workflow

1. Build firmware with instrumentation enabled
2. Load firmware to Pico
3. Run `make measure` to capture logs
4. Run `make analyze` to generate reports

See the root `Makefile` for measurement targets.

