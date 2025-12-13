#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>
#include "Arduino.h"

// I2C Pin selection constants
typedef enum {
    I2C_PINS_P32_P33 = 0,  // SCL=P3.2, SDA=P3.3 (default)
    I2C_PINS_P54_P55 = 1   // SCL=P5.4, SDA=P5.5
} I2cPinSelect_t;

// I2C Clock speed presets (in Hz) - use defines instead of enum to avoid int overflow
#define I2C_CLOCK_100KHZ 100000UL   // Standard mode
#define I2C_CLOCK_400KHZ 400000UL   // Fast mode

// I2C interface structure
typedef struct {
    void (*begin)(void);                                          // Initialize with default pins and 100kHz
    void (*beginWithPins)(I2cPinSelect_t pins);                  // Initialize with custom pins, 100kHz
    void (*end)(void);                                           // Disable I2C
    void (*setClock)(uint32_t frequency) __reentrant;            // Set I2C clock frequency
    uint8_t (*read)(void);                                       // Read one byte
    void (*write)(uint8_t data);                                 // Write one byte
    void (*beginTransmission)(uint8_t address);                  // Start transmission to slave
    uint8_t (*endTransmission)(bool sendStop);                   // End transmission
    uint8_t (*requestFrom)(uint8_t address, uint8_t quantity, bool sendStop) __reentrant; // Request bytes from slave
    uint8_t (*available)(void);                                  // Bytes available to read
} I2C_t;

// External I2C object
extern I2C_t Wire;

#endif // I2C_H