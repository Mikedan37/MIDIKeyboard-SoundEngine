# Hardware Drivers

This directory contains hardware drivers for the QWERTY MIDI Keyboard firmware.

## Structure

### `current/` - Active Drivers

These are the current, actively maintained drivers:

- **`velocity_matrix.c/h`** - Velocity-sensitive keyboard matrix scanner
 - 2-phase scanning (early/late contact detection)
 - Velocity calculation (1-127 MIDI range)
 - Debouncing and state management

- **`msqt32_shift_register.c/h`** - MSQT32 shift register interface
 - 24-bit column reading
 - DATA, CLOCK, LATCH pin control
 - Sequential bit reading

- **`gpio_driver.c/h`** - GPIO abstraction layer
 - Unified GPIO interface
 - Key state management
 - Velocity retrieval

### `legacy/` - Legacy Drivers

These drivers are kept for reference but are not actively used:

- **`keyboard_matrix.c/h`** - Legacy matrix driver (replaced by velocity_matrix)
- **`i2c_gpio_expander.c/h`** - Legacy I2C expander driver (replaced by MSQT32)
- **`fake_input.c/h`** - Test input simulation (moved to tests/)

## Usage

Include the current drivers in your firmware:

```c
#include "drivers/current/velocity_matrix.h"
#include "drivers/current/msqt32_shift_register.h"
#include "drivers/current/gpio_driver.h"
```

## Documentation

See [../../docs/hardware/](../../docs/hardware/) for detailed hardware documentation.
