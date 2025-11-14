#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "Arduino.h"  // Include for bit manipulation macros

// Pin mapping helpers
#define GET_PORT(pin)  ((pin) >> 3)
#define GET_BIT(pin)   ((pin) & 0x07)
#define GET_MASK(pin)  (1 << GET_BIT(pin))

// pinMode macro
#define pinMode(pin, mode) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _bit = GET_BIT(pin); \
    uint8_t _mask = GET_MASK(pin); \
    SET_BIT(P_SW2, 7); \
    if (_port == 2) { \
        SET_BIT_MASK(P3IE, _mask); \
        if ((mode) == OUTPUT) { \
            CLEAR_BIT(P3M1, _bit); \
            SET_BIT(P3M0, _bit); \
            CLEAR_BIT(P3PU, _bit); \
        } else if ((mode) == INPUT_PULLUP) { \
            SET_BIT(P3M1, _bit); \
            CLEAR_BIT(P3M0, _bit); \
            SET_BIT(P3PU, _bit); \
        } else { \
            SET_BIT(P3M1, _bit); \
            CLEAR_BIT(P3M0, _bit); \
            CLEAR_BIT(P3PU, _bit); \
        } \
    } else if (_port == 3) { \
        SET_BIT_MASK(P5IE, _mask); \
        if ((mode) == OUTPUT) { \
            CLEAR_BIT(P5M1, _bit); \
            SET_BIT(P5M0, _bit); \
            CLEAR_BIT(P5PU, _bit); \
        } else if ((mode) == INPUT_PULLUP) { \
            SET_BIT(P5M1, _bit); \
            CLEAR_BIT(P5M0, _bit); \
            SET_BIT(P5PU, _bit); \
        } else { \
            SET_BIT(P5M1, _bit); \
            CLEAR_BIT(P5M0, _bit); \
            CLEAR_BIT(P5PU, _bit); \
        } \
    } \
    CLEAR_BIT(P_SW2, 7); \
} while(0)

// digitalWrite macro
#define digitalWrite(pin, value) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _bit = GET_BIT(pin); \
    if (_port == 2) { \
        WRITE_BIT(P3, _bit, value); \
    } else if (_port == 3) { \
        WRITE_BIT(P5, _bit, value); \
    } \
} while(0)

// digitalRead macro
#define digitalRead(pin) \
    ((GET_PORT(pin) == 2) ? (IS_BIT_SET(P3, GET_BIT(pin)) ? HIGH : LOW) : \
     (GET_PORT(pin) == 3) ? (IS_BIT_SET(P5, GET_BIT(pin)) ? HIGH : LOW) : LOW)

// digitalToggle macro
#define digitalToggle(pin) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _bit = GET_BIT(pin); \
    if (_port == 2) { \
        TOGGLE_BIT(P3, _bit); \
    } else if (_port == 3) { \
        TOGGLE_BIT(P5, _bit); \
    } \
} while(0)

#endif // GPIO_H