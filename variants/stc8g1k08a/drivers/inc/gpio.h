#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "Arduino.h"  // Include for bit manipulation macros

// Pin mapping helpers
#define GET_PORT(pin)  (((pin) < 4) ? PORT3 : PORT5)  // Pins 0-3 → port 3, 4+ → port 5
#define GET_MASK(pin)  (1 << (pin))  // Direct mask from pin number

// pinMode macro
#define pinMode(pin, mode) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _mask = GET_MASK(pin); \
    SET_BIT(P_SW2, 7); \
    if (_port == PORT3) { \
        SET_BIT_MASK(P3IE, _mask); \
        if ((mode) == OUTPUT) { \
            CLEAR_BIT(P3M1, (pin)); \
            SET_BIT(P3M0, (pin)); \
            CLEAR_BIT(P3PU, (pin)); \
        } else if ((mode) == INPUT_PULLUP) { \
            SET_BIT(P3M1, (pin)); \
            CLEAR_BIT(P3M0, (pin)); \
            SET_BIT(P3PU, (pin)); \
        } else { \
            SET_BIT(P3M1, (pin)); \
            CLEAR_BIT(P3M0, (pin)); \
            CLEAR_BIT(P3PU, (pin)); \
        } \
    } else if (_port == PORT5) { \
        SET_BIT_MASK(P5IE, _mask); \
        if ((mode) == OUTPUT) { \
            CLEAR_BIT(P5M1, (pin)); \
            SET_BIT(P5M0, (pin)); \
            CLEAR_BIT(P5PU, (pin)); \
        } else if ((mode) == INPUT_PULLUP) { \
            SET_BIT(P5M1, (pin)); \
            CLEAR_BIT(P5M0, (pin)); \
            SET_BIT(P5PU, (pin)); \
        } else { \
            SET_BIT(P5M1, (pin)); \
            CLEAR_BIT(P5M0, (pin)); \
            CLEAR_BIT(P5PU, (pin)); \
        } \
    } \
    CLEAR_BIT(P_SW2, 7); \
} while(0)

// digitalWrite macro
#define digitalWrite(pin, value) do { \
    uint8_t _port = GET_PORT(pin); \
    if (_port == PORT3) { \
        WRITE_BIT(P3, (pin), value); \
    } else if (_port == PORT5) { \
        WRITE_BIT(P5, (pin), value); \
    } \
} while(0)

// digitalRead macro
#define digitalRead(pin) \
    ((GET_PORT(pin) == PORT3) ? (IS_BIT_SET(P3, (pin)) ? HIGH : LOW) : \
     (GET_PORT(pin) == PORT5) ? (IS_BIT_SET(P5, (pin)) ? HIGH : LOW) : LOW)

// digitalToggle macro
#define digitalToggle(pin) do { \
    uint8_t _port = GET_PORT(pin); \
    if (_port == PORT3) { \
        TOGGLE_BIT(P3, (pin)); \
    } else if (_port == PORT5) { \
        TOGGLE_BIT(P5, (pin)); \
    } \
} while(0)

#endif // GPIO_H