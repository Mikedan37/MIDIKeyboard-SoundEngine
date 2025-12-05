#ifndef I2C_GPIO_EXPANDER_H
#define I2C_GPIO_EXPANDER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// MCP23017 I2C addresses (adjust based on your hardware)
// Typical addresses: 0x20-0x27 (set by A0, A1, A2 pins)
#define EXPANDER_U2_ADDR 0x20  // A0=0, A1=0, A2=0
#define EXPANDER_U3_ADDR 0x21  // A0=1, A1=0, A2=0

// MCP23017 Register addresses
#define MCP23017_IODIRA   0x00  // I/O Direction Register A
#define MCP23017_IODIRB   0x01  // I/O Direction Register B
#define MCP23017_IPOLA    0x02  // Input Polarity Register A
#define MCP23017_IPOLB    0x03  // Input Polarity Register B
#define MCP23017_GPINTENA 0x04  // Interrupt-on-Change Register A
#define MCP23017_GPINTENB 0x05  // Interrupt-on-Change Register B
#define MCP23017_DEFVALA  0x06  // Default Value Register A
#define MCP23017_DEFVALB  0x07  // Default Value Register B
#define MCP23017_INTCONA  0x08  // Interrupt Control Register A
#define MCP23017_INTCONB  0x09  // Interrupt Control Register B
#define MCP23017_IOCON    0x0A  // Configuration Register
#define MCP23017_GPPUA    0x0C  // Pull-up Register A
#define MCP23017_GPPUB    0x0D  // Pull-up Register B
#define MCP23017_INTFA    0x0E  // Interrupt Flag Register A
#define MCP23017_INTFB    0x0F  // Interrupt Flag Register B
#define MCP23017_INTCAPA  0x10  // Interrupt Capture Register A
#define MCP23017_INTCAPB  0x11  // Interrupt Capture Register B
#define MCP23017_GPIOA    0x12  // GPIO Register A
#define MCP23017_GPIOB    0x13  // GPIO Register B
#define MCP23017_OLATA    0x14  // Output Latch Register A
#define MCP23017_OLATB    0x15  // Output Latch Register B

// I2C Configuration
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4   // GPIO4
#define I2C_SCL_PIN 5   // GPIO5
#define I2C_FREQ 400000 // 400kHz

// Function prototypes
void i2c_expander_init(void);
bool i2c_expander_read_port(uint8_t addr, uint8_t port, uint8_t *data);
bool i2c_expander_write_port(uint8_t addr, uint8_t port, uint8_t data);
bool i2c_expander_set_direction(uint8_t addr, uint8_t port, uint8_t direction);
bool i2c_expander_set_pullup(uint8_t addr, uint8_t port, uint8_t pullup);
bool i2c_expander_read_pin(uint8_t addr, uint8_t port, uint8_t pin, bool *state);
bool i2c_expander_write_pin(uint8_t addr, uint8_t port, uint8_t pin, bool state);

#endif

