#include "i2c_gpio_expander.h"
#include <stdio.h>

void i2c_expander_init(void) {
    // Initialize I2C
    i2c_init(I2C_PORT, I2C_FREQ);
    
    // Set GPIO functions
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    
    // Enable pull-ups
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    // Configure both expanders
    // Set all pins as inputs initially with pull-ups enabled
    i2c_expander_set_direction(EXPANDER_U2_ADDR, 0, 0xFF); // Port A = inputs
    i2c_expander_set_direction(EXPANDER_U2_ADDR, 1, 0xFF); // Port B = inputs
    i2c_expander_set_pullup(EXPANDER_U2_ADDR, 0, 0xFF);
    i2c_expander_set_pullup(EXPANDER_U2_ADDR, 1, 0xFF);
    
    i2c_expander_set_direction(EXPANDER_U3_ADDR, 0, 0xFF);
    i2c_expander_set_direction(EXPANDER_U3_ADDR, 1, 0xFF);
    i2c_expander_set_pullup(EXPANDER_U3_ADDR, 0, 0xFF);
    i2c_expander_set_pullup(EXPANDER_U3_ADDR, 1, 0xFF);
    
    printf("[I2C] Initialized expanders U2(0x%02X) and U3(0x%02X)\n", 
           EXPANDER_U2_ADDR, EXPANDER_U3_ADDR);
}

bool i2c_expander_write_port(uint8_t addr, uint8_t port, uint8_t data) {
    uint8_t buf[2];
    buf[0] = port;
    buf[1] = data;
    
    int ret = i2c_write_blocking(I2C_PORT, addr, buf, 2, false);
    if (ret != 2) {
        return false;
    }
    return true;
}

bool i2c_expander_read_port(uint8_t addr, uint8_t port, uint8_t *data) {
    uint8_t reg = port;
    
    // Write register address
    if (i2c_write_blocking(I2C_PORT, addr, &reg, 1, true) != 1) {
        return false;
    }
    
    // Read data
    if (i2c_read_blocking(I2C_PORT, addr, data, 1, false) != 1) {
        return false;
    }
    
    return true;
}

bool i2c_expander_set_direction(uint8_t addr, uint8_t port, uint8_t direction) {
    // 0 = output, 1 = input
    uint8_t reg = (port == 0) ? MCP23017_IODIRA : MCP23017_IODIRB;
    return i2c_expander_write_port(addr, reg, direction);
}

bool i2c_expander_set_pullup(uint8_t addr, uint8_t port, uint8_t pullup) {
    uint8_t reg = (port == 0) ? MCP23017_GPPUA : MCP23017_GPPUB;
    return i2c_expander_write_port(addr, reg, pullup);
}

bool i2c_expander_read_pin(uint8_t addr, uint8_t port, uint8_t pin, bool *state) {
    if (pin > 7) return false;
    
    uint8_t port_data;
    if (!i2c_expander_read_port(addr, (port == 0) ? MCP23017_GPIOA : MCP23017_GPIOB, &port_data)) {
        return false;
    }
    
    *state = (port_data >> pin) & 1;
    return true;
}

bool i2c_expander_write_pin(uint8_t addr, uint8_t port, uint8_t pin, bool state) {
    if (pin > 7) return false;
    
    uint8_t reg = (port == 0) ? MCP23017_OLATA : MCP23017_OLATB;
    uint8_t port_data;
    
    // Read current state
    if (!i2c_expander_read_port(addr, reg, &port_data)) {
        return false;
    }
    
    // Modify bit
    if (state) {
        port_data |= (1 << pin);
    } else {
        port_data &= ~(1 << pin);
    }
    
    // Write back
    return i2c_expander_write_port(addr, reg, port_data);
}

