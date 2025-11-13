#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

// Pin mapping helpers
#define GET_PORT(pin)  ((pin) >> 3)
#define GET_BIT(pin)   ((pin) & 0x07)
#define GET_MASK(pin)  (1 << GET_BIT(pin))

// pinMode macro
#define pinMode(pin, mode) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _mask = GET_MASK(pin); \
    P_SW2 = 0x80; \
    if (_port == 2) { \
        P3IE |= _mask; \
        if ((mode) == OUTPUT) { \
            P3M1 &= ~_mask; P3M0 |= _mask; P3PU &= ~_mask; \
        } else if ((mode) == INPUT_PULLUP) { \
            P3M1 |= _mask; P3M0 &= ~_mask; P3PU |= _mask; \
        } else { \
            P3M1 |= _mask; P3M0 &= ~_mask; P3PU &= ~_mask; \
        } \
    } else if (_port == 3) { \
        P5IE |= _mask; \
        if ((mode) == OUTPUT) { \
            P5M1 &= ~_mask; P5M0 |= _mask; P5PU &= ~_mask; \
        } else if ((mode) == INPUT_PULLUP) { \
            P5M1 |= _mask; P5M0 &= ~_mask; P5PU |= _mask; \
        } else { \
            P5M1 |= _mask; P5M0 &= ~_mask; P5PU &= ~_mask; \
        } \
    } \
    P_SW2 = 0x00; \
} while(0)

// digitalWrite macro
#define digitalWrite(pin, value) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _mask = GET_MASK(pin); \
    if (_port == 2) { \
        if (value) P3 |= _mask; else P3 &= ~_mask; \
    } else if (_port == 3) { \
        if (value) P5 |= _mask; else P5 &= ~_mask; \
    } \
} while(0)

// digitalRead macro
#define digitalRead(pin) \
    ((GET_PORT(pin) == 2) ? ((P3 & GET_MASK(pin)) ? HIGH : LOW) : \
     (GET_PORT(pin) == 3) ? ((P5 & GET_MASK(pin)) ? HIGH : LOW) : LOW)

// digitalToggle macro
#define digitalToggle(pin) do { \
    uint8_t _port = GET_PORT(pin); \
    uint8_t _mask = GET_MASK(pin); \
    if (_port == 2) P3 ^= _mask; \
    else if (_port == 3) P5 ^= _mask; \
} while(0)

#endif // GPIO_H