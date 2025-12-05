# Test Suite

Comprehensive test suite for the QWERTY MIDI Keyboard firmware.

## Overview

Tests validate the firmware logic **without requiring hardware**. All tests use simulated input and mock functions.

## Test Files

- **`test_velocity_simple.c`** - Velocity calculation algorithm tests
 - Tests time-to-velocity conversion
 - Validates velocity range (1-127)
 - Verifies edge cases

- **`test_velocity_matrix.c`** - Full matrix scanning tests (requires mocking)
- **`mock_pico.c/h`** - Mock implementations of Pico SDK functions
- **`mock_pico_headers.h`** - Mock header definitions

## Running Tests

```bash
cd qwerty_midi_pico/tests
make test_velocity_simple
./test_velocity_simple
```

## Documentation

See [../../docs/testing/](../../docs/testing/) for complete testing documentation.

## Test Coverage

- Velocity calculation algorithm
- Time-to-velocity conversion
- Full velocity range (1-127)
- Edge case handling
- Monotonicity verification
