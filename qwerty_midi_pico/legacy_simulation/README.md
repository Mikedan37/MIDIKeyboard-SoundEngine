# Legacy Simulation Code

This directory contains legacy simulation code from earlier development phases.

## Status: Legacy

This code is **not actively maintained** and is kept for reference only.

## Contents

- `sim_main.c` - Legacy simulation main
- `gpio_scanner.c` - Legacy GPIO scanning
- `tonegen.c/h` - Legacy tone generation
- `test_poly.c` - Legacy polyphonic test
- `test_tone.c` - Legacy tone test
- `key_map.h` - Legacy key mapping
- `sim_hardware.h` - Legacy hardware simulation

## Migration

The functionality has been migrated to:
- **Current firmware**: `main.c` and `drivers/current/`
- **Current tests**: `tests/test_velocity_simple.c`

## Documentation

See [../../docs/](../../docs/) for current documentation.
