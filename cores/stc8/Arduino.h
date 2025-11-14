#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdbool.h>
#include "variant.h"
#include "HardwareSerial.h"

// Arduino Pin Definitions (map to STC8G pins)
#define LED_BUILTIN 13

// Pin modes
#define INPUT           0x00
#define OUTPUT          0x01
#define INPUT_PULLUP    0x02

// Digital values
#define LOW     0
#define HIGH    1

// ====================================================================================
// BIT MANIPULATION MACROS
// ====================================================================================

// Set a specific bit in a register/variable
#define SET_BIT(reg, bit)    ((reg) |= (1 << (bit)))

// Set BIT with value
#define SET_BIT_VAL(reg, value, bit) (reg |= ((value) << (bit)))

// Clear a specific bit in a register/variable
#define CLEAR_BIT(reg, bit)  ((reg) &= ~(1 << (bit)))

// Clear BIT with value
#define CLEAR_BIT_VAL(reg, value, bit) (reg &= ~((value) << (bit)))

// Toggle a specific bit in a register/variable
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1 << (bit)))

// Read a specific bit from a register/variable (returns 0 or non-zero)
#define READ_BIT(reg, bit)   ((reg) & (1 << (bit)))

// Check if a bit is set (returns 1 if set, 0 if clear)
#define IS_BIT_SET(reg, bit) (((reg) & (1 << (bit))) != 0)

// Check if a bit is clear (returns 1 if clear, 0 if set)
#define IS_BIT_CLEAR(reg, bit) (((reg) & (1 << (bit))) == 0)

// Write a specific bit value (0 or 1)
#define WRITE_BIT(reg, bit, value) do { \
    if (value) SET_BIT(reg, bit); \
    else CLEAR_BIT(reg, bit); \
} while(0)

// Set multiple bits using a mask
#define SET_BIT_MASK(reg, mask) ((reg) |= (mask))

// Clear multiple bits using a mask
#define CLEAR_BIT_MASK(reg, mask) ((reg) &= ~(mask))

// Toggle multiple bits using a mask
#define TOGGLE_BIT_MASK(reg, mask) ((reg) ^= (mask))

// Read multiple bits using a mask
#define READ_BIT_MASK(reg, mask) ((reg) & (mask))

// Byte manipulation macros
#define LOW_BYTE(w)  ((uint8_t)((w) & 0xFF))
#define HIGH_BYTE(w) ((uint8_t)((w) >> 8))

// Make word from two bytes
#define MAKE_WORD(h, l) ((uint16_t)(((h) << 8) | (l)))

// ====================================================================================

// PinMap structure
typedef struct {
    uint8_t port;
    uint8_t bit;
    uint8_t mask;
} PinMap;

void clock_init(void);

// GPIO functions
// void pinMode(uint8_t pin, uint8_t mode);
// void digitalWrite(uint8_t pin, uint8_t value);
// uint8_t digitalRead(uint8_t pin);

// Timer 0 mode definitions
// void timer0_init();
// void delay_us(uint32_t us);
// void delay_ms(uint16_t ms);
// void delay_s(uint8_t seconds);
uint32_t micros(void);
uint32_t millis(void);

void init(void);

// Setup and loop (Arduino standard)
extern void setup(void);
extern void loop(void);

#endif // Arduino_h