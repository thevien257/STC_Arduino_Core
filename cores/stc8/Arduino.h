#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdbool.h>
#include "variant.h"
#include "HardwareSerial.h"
#include "interrupt.h"

// Arduino Pin Definitions (map to STC8G pins)
#define LED_BUILTIN 13

// Pin mode definitions
#define INPUT           0x00
#define OUTPUT          0x01
#define INPUT_PULLUP    0x02
#define OUTPUT_OD       0x03  // Open-drain output
#define OUTPUT_OD_PU    0x04  // Open-drain output with pull-up

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
// EXTENDED RAM/SFR ACCESS (XFR) HELPERS
// ====================================================================================

// Enable access to extended SFRs (XDATA registers like I2C, clock, etc.)
#define ENABLE_XFR()  SET_BIT(P_SW2, 7)

// Disable extended SFR access
#define DISABLE_XFR() CLEAR_BIT(P_SW2, 7)

// ====================================================================================
// GENERIC PERIPHERAL PIN SWITCH HELPERS
// ====================================================================================

// Switch peripheral pins (P_SW1 register - UART1, CCP, SPI)
#define SWITCH_PERIPHERAL_P_SW1(mask, value) do { \
    P_SW1 = (P_SW1 & ~(mask)) | (value); \
} while(0)

// Switch peripheral pins (P_SW2 register - UART2/3/4, I2C, Comparator)
#define SWITCH_PERIPHERAL_P_SW2(mask, value) do { \
    uint8_t _xfr_backup = P_SW2 & 0x80; \
    ENABLE_XFR(); \
    P_SW2 = (P_SW2 & ~(mask)) | (value); \
    if (!_xfr_backup) DISABLE_XFR(); \
} while(0)

// ====================================================================================
// SPECIFIC PERIPHERAL PIN SWITCH HELPERS
// ====================================================================================

// UART1 Pin Switch (uses P_SW1 bits 7:6)
#define UART1_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW1(0xC0, pins)

// I2C Pin Switch (uses P_SW2 bits 5:4)
#define I2C_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW2(0x30, pins)

// SPI Pin Switch (uses P_SW1 bits 3:2)
#define SPI_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW1(0x0C, pins)

// CCP/PCA Pin Switch (uses P_SW1 bits 5:4)
#define CCP_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW1(0x30, pins)

// UART2 Pin Switch (uses P_SW2 bit 0)
#define UART2_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW2(0x01, pins)

// UART3 Pin Switch (uses P_SW2 bit 1)
#define UART3_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW2(0x02, pins)

// UART4 Pin Switch (uses P_SW2 bit 2)
#define UART4_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW2(0x04, pins)

// Comparator Pin Switch (uses P_SW2 bit 3)
#define CMPO_SWITCH_PINS(pins) SWITCH_PERIPHERAL_P_SW2(0x08, pins)

// ====================================================================================
// SYSTEM FUNCTIONS
// ====================================================================================

void clock_init(void);

// Timer functions
uint32_t micros(void);
uint32_t millis(void);

void init(void);

// Setup and loop (Arduino standard)
extern void setup(void);
extern void loop(void);

#endif // Arduino_h